// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGameInstance.h"

UARGameInstance::UARGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GSObject = ObjectInitializer.CreateDefaultSubobject<UGameSparksObject>(this, TEXT("GSObject"));
	GSMessageListener = ObjectInitializer.CreateDefaultSubobject<UGSMessageListenersObject>(this, TEXT("GSMessageListener"));
}


void UARGameInstance::OnGameSparksAvailable(bool bAvailable)
{
	if (bAvailable)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Connected"));

		GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();

		GameSparks::Api::Requests::AuthenticationRequest authRequest(gs);
		authRequest.SetUserName("");
		authRequest.SetPassword("");

		authRequest.Send(AuthenticationRequest_Response);
	}

}

//The response function to the Authentication Request
// Example response function
void UARGameInstance::AuthenticationRequest_Response(GameSparks::Core::GS&, const GameSparks::Api::Responses::AuthenticationResponse& response)
{
	GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, response.GetJSONString().c_str());
	//Check is response has no errors
	if (!response.GetHasErrors()) {
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Auth response successful"));

		GameSparks::Core::GS& gs = UGameSparksModule::GetModulePtr()->GetGSInstance();
		GameSparks::Api::Requests::AccountDetailsRequest accDetRequest(gs);
		//If no errors then send an accounts details request
		accDetRequest.Send();

	}
}