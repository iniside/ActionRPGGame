// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "UI/ARUMGWidgetBase.h"
#include "ARLoginScreenView.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARLoginScreenView : public UARUMGWidgetBase
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* UserNameBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UEditableTextBox* PasswordBox;
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* LoginButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UButton* RegisterButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UTextBlock* WarrningText;

	UFUNCTION()
		void OnLoginClicked();

	UFUNCTION()
		void OnLoginSuccess();

	UFUNCTION()
		void OnLoginFailed();

	UFUNCTION()
		void OnRegisterClicked();
};
