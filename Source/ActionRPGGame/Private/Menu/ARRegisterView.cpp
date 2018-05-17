// Fill out your copyright notice in the Description page of Project Settings.

#include "ARRegisterView.h"
#include "ARGameInstance.h"



void UARRegisterView::NativeConstruct()
{
	Super::NativeConstruct();

	RegisterButton->OnClicked.AddDynamic(this, &UARRegisterView::OnRegisterClicked);
}


void UARRegisterView::OnRegisterClicked()
{
	FString UserName = UserNameBox->GetText().ToString();
	FString DisplayName = DisplayNameBox->GetText().ToString();
	FString Password = PasswordBox->GetText().ToString();

	if (UserName.Len() <= 0)
	{
		WarrningText->SetText(FText::FromString("Invalid Username"));
		return;
	}
	if (DisplayName.Len() <= 0)
	{
		WarrningText->SetText(FText::FromString("Invalid DisplayName"));
		return;
	}
	if (Password.Len() <= 0)
	{
		WarrningText->SetText(FText::FromString("Invalid Password"));
		return;
	}

	if (UARGameInstance* GI = Cast<UARGameInstance>(GetOwningPlayer()->GetGameInstance()))
	{
		RegisterButton->SetVisibility(ESlateVisibility::HitTestInvisible);
		GI->RegisterNewPlayer(UserName, DisplayName, Password);
	}
}

void UARRegisterView::OnRegisterSuccess()
{

}

void UARRegisterView::OnRegisterFailed()
{

}