// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameInstance.h"

UARGameInstance::UARGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GSObject = ObjectInitializer.CreateDefaultSubobject<UGameSparksObject>(this, TEXT("GSObject"));
	GSMessageListener = ObjectInitializer.CreateDefaultSubobject<UGSMessageListenersObject>(this, TEXT("GSMessageListener"));
#if WITH_EDITOR
	bConnected = false;
#endif
}


void UARGameInstance::OnGameSparksAvailable(bool bAvailable)
{
#if WITH_EDITOR
	bConnected = false;
#endif
	OnConnectedToGameSparks.Broadcast();
}

//The response function to the Authentication Request
// Example response function
void UARGameInstance::AuthenticationRequest_Response(GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse& response)
{

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
