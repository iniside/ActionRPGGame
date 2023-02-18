// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "ARRegisterView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARRegisterView : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* UserNameBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* DisplayNameBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* PasswordBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* RegisterButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* WarrningText;
	

	UFUNCTION()
		void OnRegisterClicked();

	UFUNCTION()
		void OnRegisterSuccess();

	UFUNCTION()
		void OnRegisterFailed();
};
