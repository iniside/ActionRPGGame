// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARUMGWidgetBase.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARUMGWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		class UARUIComponent* UIComponent;
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	
	
};
