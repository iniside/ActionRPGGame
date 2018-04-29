// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetTree.h"
#include "Components/UniformGridPanel.h"

#include "UI/HUD/ARHUDPlayerInfo.h"
#include "ARHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARHUDWidget : public UUserWidget
{
	GENERATED_BODY()
protected:

	TWeakObjectPtr<class UARUIComponent> UIComponent;
public:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UARHUDPlayerInfo* PlayerInfo;
};
