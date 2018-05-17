// Fill out your copyright notice in the Description page of Project Settings.

#include "ARMenuHUD.h"

#include "UI/Menu/ARLoginScreenView.h"
#include "ARGameInstance.h"
#include "ARMainMenuView.h"
#include "ARRegisterView.h"


void AARMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (LoginScreenClass)
		{
			LoginScreen = CreateWidget<UARLoginScreenView>(PC, LoginScreenClass);
			LoginScreen->AddToViewport();
		}

		if (MainMenuScreenClass)
		{
			MainMenuScreen = CreateWidget<UARMainMenuView>(PC, MainMenuScreenClass);
			MainMenuScreen->AddToViewport();

			MainMenuScreen->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (RegisterViewClass)
		{
			RegisterView = CreateWidget<UARRegisterView>(PC, RegisterViewClass);
			RegisterView->AddToViewport();

			RegisterView->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (UARGameInstance* GI = Cast<UARGameInstance>(PC->GetGameInstance()))
		{
			GI->OnLoginSuccess.AddDynamic(this, &AARMenuHUD::OnLoginSuccess);
		}
	}
}

void AARMenuHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (LoginScreen)
	{
		LoginScreen->RemoveFromParent();
		LoginScreen->RemoveFromViewport();
		LoginScreen->SetVisibility(ESlateVisibility::Collapsed);
		LoginScreen->MarkPendingKill();
	}

	if (MainMenuScreen)
	{
		MainMenuScreen->RemoveFromParent();
		MainMenuScreen->RemoveFromViewport();
		MainMenuScreen->SetVisibility(ESlateVisibility::Collapsed);
		MainMenuScreen->MarkPendingKill();
	}
}

void AARMenuHUD::OnLoginSuccess()
{
	if (APlayerController* PC = GetOwningPlayerController())
	{
		if (UARGameInstance* GI = Cast<UARGameInstance>(PC->GetGameInstance()))
		{
			GI->OnLoginSuccess.RemoveDynamic(this, &AARMenuHUD::OnLoginSuccess);

			LoginScreen->SetVisibility(ESlateVisibility::Collapsed);
			MainMenuScreen->SetVisibility(ESlateVisibility::Visible);
		}
	}
}