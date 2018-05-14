// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameInstance.h"
#include "Engine/World.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "Json.h"


UARGameInstance::UARGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GSObject = ObjectInitializer.CreateDefaultSubobject<UGameSparksObject>(this, TEXT("GSObject"));
	GSMessageListener = ObjectInitializer.CreateDefaultSubobject<UGSMessageListenersObject>(this, TEXT("GSMessageListener"));
#if WITH_EDITOR
	bConnected = false;
#endif

#if GAMELIFT_CLIENT
	AccessKey = "Local";
	Secret = "Local";
	Region = "Local";
	LocalPort = 8080;
	bIsUsingGameLiftLocal = true;
#endif
}


void UARGameInstance::OnGameSparksAvailable(bool bAvailable)
{
#if WITH_EDITOR
	bConnected = false;
#endif
	OnConnectedToGameSparks.Broadcast();
}

void UARGameInstance::AttemptLogin(const FString& UserName, const FString& Password)
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Connected"));

	GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();

	GameSparks::Api::Requests::AuthenticationRequest authRequest(gs);
	authRequest.SetUserName(TCHAR_TO_UTF8(*UserName));
	authRequest.SetPassword(TCHAR_TO_UTF8(*Password));

	auto AuthRquestResponse = [&](GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse& response)
	{
		GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, response.GetJSONString().c_str());
		//Check is response has no errors
		if (!response.GetHasErrors())
		{
			OnLoginSuccess.Broadcast();
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Auth response successful"));

			GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
			GameSparks::Api::Requests::AccountDetailsRequest accDetRequest(gs);
			//If no errors then send an accounts details request
			accDetRequest.Send();
		}
		else
		{
			OnLoginFailed.Broadcast();
		}
	};

	authRequest.Send(AuthRquestResponse);
	
}

void UARGameInstance::Init()
{
	Super::Init();
#if WITH_EDITOR
	//editor hax so we don't attempt to connect twice.
	if (!bConnected)
#endif
	{
		//Set the OnAvailable delegate
		GetGSObject()->OnGameSparksAvailableDelegate.AddDynamic(this, &UARGameInstance::OnGameSparksAvailable);
		//Disconnected the module just incase it's connected (Refresh)
		GetGSObject()->Disconnect();
		//Connect module
		GetGSObject()->Connect(GSKey, GSSecret);

#if WITH_EDITOR
		bConnected = true; //it will be overriden in delegate anyway.
#endif
	}
#if LINUX_GAMELIFT
	InitGameLift();
#endif

#if GAMELIFT_CLIENT
	Aws::Client::ClientConfiguration ClientConfig;
	Aws::Auth::AWSCredentials Credentials;

	ClientConfig.connectTimeoutMs = 10000;
	ClientConfig.requestTimeoutMs = 10000;
	ClientConfig.region = TCHAR_TO_UTF8(*Region);

	// GameLiftLocal
	if (bIsUsingGameLiftLocal)
	{
		ClientConfig.scheme = Aws::Http::Scheme::HTTP;
		const FString HostAddress = FString::Printf(TEXT("localhost:%i"), LocalPort);
		ClientConfig.endpointOverride = TCHAR_TO_UTF8(*HostAddress);
		UE_LOG(LogTemp, Warning, TEXT("GameLift is currently configured to use GameLift Local."));
		
	}

	Credentials = Aws::Auth::AWSCredentials(TCHAR_TO_UTF8(*AccessKey), TCHAR_TO_UTF8(*Secret));
	GameLiftClient = new Aws::GameLift::GameLiftClient(Credentials, ClientConfig);
#endif

}
#if WITH_EDITOR

/* Called to actually start the game when doing Play/Simulate In Editor */
FGameInstancePIEResult UARGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer, const FGameInstancePIEParameters& Params)
{
	if (!bConnected)

	{
		//Set the OnAvailable delegate
		GetGSObject()->OnGameSparksAvailableDelegate.AddDynamic(this, &UARGameInstance::OnGameSparksAvailable);
		//Disconnected the module just incase it's connected (Refresh)
		GetGSObject()->Disconnect();
		//Connect module
		GetGSObject()->Connect(GSKey, GSSecret);
		bConnected = true; //it will be overriden in delegate anyway.
	}
	
	return Super::StartPlayInEditorGameInstance(LocalPlayer, Params);
}

#endif

#if LINUX_GAMELIFT
void UARGameInstance::InitGameLift()
{
	//Getting the module first.
	FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

	//InitSDK establishes a local connection with GameLift's agent to enable further communication.
	gameLiftSdkModule->InitSDK();

	//When a game session is created, GameLift sends an activation request to the game server and passes along the game session object containing game properties and other settings.
	//Here is where a game server should take action based on the game session object.
	//Once the game server is ready to receive incoming player connections, it should invoke GameLiftServerAPI.ActivateGameSession()
	auto onGameSession = [=](Aws::GameLift::Server::Model::GameSession gameSession)
	{
		this->OnGameSessionStarted(gameSession);
	};

	FProcessParameters* params = new FProcessParameters();
	params->OnStartGameSession.BindLambda(onGameSession);

	//OnProcessTerminate callback. GameLift invokes this callback before shutting down an instance hosting this game server.
	//It gives this game server a chance to save its state, communicate with services, etc., before being shut down.
	//In this case, we simply tell GameLift we are indeed going to shut down.
	params->OnTerminate.BindLambda([=]() {gameLiftSdkModule->ProcessEnding(); });

	//This is the HealthCheck callback.
	//GameLift invokes this callback every 60 seconds or so.
	//Here, a game server might want to check the health of dependencies and such.
	//Simply return true if healthy, false otherwise.
	//The game server has 60 seconds to respond with its health status. GameLift defaults to 'false' if the game server doesn't respond in time.
	//In this case, we're always healthy!
	params->OnHealthCheck.BindLambda([]() {return true; });
	//FString Address = GetWorld()->GetAddressURL();
	//FString portString = Address.RightChop(4);
	int32 port = GetWorld()->URL.Port; //FCString::Atoi(*portString);
	UE_LOG(LogTemp, Warning, TEXT("Current Server Address %d "), port);
	//This game server tells GameLift that it listens on port 7777 for incoming player connections.
	params->port = port;

	//Here, the game server tells GameLift what set of files to upload when the game session ends.
	//GameLift uploads everything specified here for the developers to fetch later.
	TArray<FString> logfiles;
	logfiles.Add(TEXT("aLogFile.txt"));
	params->logParameters = logfiles;

	//Calling ProcessReady tells GameLift this game server is ready to receive incoming game sessions!
	gameLiftSdkModule->ProcessReady(*params);

}

void UARGameInstance::OnGameSessionStarted(Aws::GameLift::Server::Model::GameSession InGameSession)
{
	auto func2 = [&]()
	{
		int outNum = 0;;
		const Aws::GameLift::Server::Model::GameProperty* props = InGameSession.GetGameProperties(outNum);
		FGameLiftServerSDKModule* gameLiftSdkModule = &FModuleManager::LoadModuleChecked<FGameLiftServerSDKModule>(FName("GameLiftServerSDK"));

		gameLiftSdkModule->ActivateGameSession();

		FString MapName = FString(props[0].GetValue());
		UE_LOG(LogTemp, Warning, TEXT("%s"), *MapName);
		UGameplayStatics::OpenLevel(this, *MapName);
	};
	AsyncTask(ENamedThreads::GameThread, func2);
}
#endif

void UARGameInstance::ConnectToHub()
{
#if GAMELIFT_CLIENT
	using namespace Aws::GameLift::Model;
	SearchGameSessionsRequest SearchRequest;
	//SearchRequest.
	Aws::GameLift::SearchGameSessionsResponseReceivedHandler SearchHandler;

	SearchHandler = std::bind(&UARGameInstance::OnSessionsSearched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

	GameLiftClient->SearchGameSessionsAsync(SearchRequest, SearchHandler);
	


#endif
}


void UARGameInstance::ConnectToWorld()
{
#if GAMELIFT_CLIENT

#endif
}

#if GAMELIFT_CLIENT

void UARGameInstance::OnSessionsSearched(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::SearchGameSessionsRequest& Request, const Aws::GameLift::Model::SearchGameSessionsOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context)
{
	if (Outcome.IsSuccess())
	{
		using namespace Aws::GameLift::Model;
		SearchGameSessionsResult SearchResult = Outcome.GetResult();

		const Aws::Vector<GameSession>& Sessions =  SearchResult.GetGameSessions();

		for (const GameSession& Session : Sessions)
		{
			if (Session.GetCurrentPlayerSessionCount() < 8)
			{
				Aws::GameLift::Model::CreatePlayerSessionRequest PlayerRequest;
				PlayerRequest.SetGameSessionId(Session.GetGameSessionId());
				PlayerRequest.SetPlayerId("TestPlayer");

				Aws::GameLift::CreatePlayerSessionResponseReceivedHandler Handler;
				Handler = std::bind(&UARGameInstance::OnCreatePlayerSession, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

				GameLiftClient->CreatePlayerSessionAsync(PlayerRequest, Handler);
				break;
			}
		}

	}
	else
	{
		using namespace Aws::GameLift::Model;
		CreateGameSessionRequest CreateRequest;
		CreateRequest.SetFleetId("Local");
		CreateRequest.SetMaximumPlayerSessionCount(8);
		GameProperty MapName;
		MapName.SetKey("MapName");
		MapName.SetValue("TestMap"); //reasons,,

		CreateRequest.AddGameProperties(MapName);

		Aws::GameLift::CreateGameSessionResponseReceivedHandler Handler;


		Handler = std::bind(&UARGameInstance::OnSessionCreated, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		GameLiftClient->CreateGameSessionAsync(CreateRequest, Handler);
	}
}

void UARGameInstance::OnSessionCreated(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::CreateGameSessionRequest& Request, const Aws::GameLift::Model::CreateGameSessionOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context)
{
	if (Outcome.IsSuccess())
	{
		using namespace Aws::GameLift::Model;
		CreateGameSessionResult Result = Outcome.GetResult();
		GameSession Session = Result.GetGameSession();

		Aws::GameLift::Model::CreatePlayerSessionRequest PlayerRequest;
		PlayerRequest.SetGameSessionId(Session.GetGameSessionId());
		PlayerRequest.SetPlayerId("TestPlayer");

		Aws::GameLift::CreatePlayerSessionResponseReceivedHandler Handler;
		Handler = std::bind(&UARGameInstance::OnCreatePlayerSession, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		GameLiftClient->CreatePlayerSessionAsync(PlayerRequest, Handler);

	}
}

void UARGameInstance::OnCreatePlayerSession(const Aws::GameLift::GameLiftClient* Client, const Aws::GameLift::Model::CreatePlayerSessionRequest& Request, const Aws::GameLift::Model::CreatePlayerSessionOutcome& Outcome, const std::shared_ptr<const Aws::Client::AsyncCallerContext>& Context)
{
	using namespace Aws::GameLift::Model;
	const FString ServerIpAddress = FString(Outcome.GetResult().GetPlayerSession().GetIpAddress().c_str());
	const FString ServerPort = FString::FromInt(Outcome.GetResult().GetPlayerSession().GetPort());

	FString FullAddress = ServerIpAddress + ":" + ServerPort;

	GEngine->SetClientTravel(GetWorld(), *FullAddress, ETravelType::TRAVEL_Absolute);
}

#endif

void UARGameInstance::TestGSRequest()
{
	GSRequestData scriptData;
	scriptData.AddString("eventKey", "SearchGameSessions");
	GameSparks::Api::Requests::LogEventRequest request(UGameSparksModule::GetModulePtr()->GetGSInstance());
	request.SetEventKey("SearchGameSessions");

	auto func = [=](GS& gs, GameSparks::Api::Responses::LogEventResponse response)
	{
		this->OnScriptMessageReceived(response);
	};
	request.Send(func);
}
void UARGameInstance::OnScriptMessageReceived(GameSparks::Api::Responses::LogEventResponse response)
{
	TSharedPtr<FJsonObject> PlayerSessions = GetGameLiftResponseFromGS("PlayerSession", response);
	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, response.GetJSONString().c_str());
	FString PlayerID = PlayerSessions->GetStringField("PlayerId");
	FString IP = PlayerSessions->GetStringField("IpAddress");
	int32 Port = PlayerSessions->GetIntegerField("Port");

	FString Options = FString::Printf(TEXT("%s:%d?PlayerId=%s"), *IP, Port, *PlayerID);

	TArray<FString> OutParams;
	Options.ParseIntoArray(OutParams, TEXT("?"));


	GEngine->SetClientTravel(GetWorld(), *Options, ETravelType::TRAVEL_Absolute);
}

TSharedPtr<FJsonObject> UARGameInstance::GetGameLiftResponseFromGS(const FString& Field, GameSparks::Api::Responses::LogEventResponse response)
{
	gsstl::string str = response.GetJSONString();
	FString data(str.data());

	UE_LOG(LogTemp, Warning, TEXT("GameLift Response Data %s "), *data);

	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(data);

	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());
	bool bSuccessful = FJsonSerializer::Deserialize(Reader, Object);

	TSharedPtr<FJsonObject> scriptData = Object->GetObjectField("scriptData");
	TSharedPtr<FJsonObject> respo = scriptData->GetObjectField("response");
	TSharedPtr<FJsonObject> searchGameSessions = respo->GetObjectField("searchGameSessions");
	TSharedPtr<FJsonObject> json = searchGameSessions->GetObjectField("json");
	TSharedPtr<FJsonObject> GameSession= json->GetObjectField(Field);
	

	return GameSession;
}