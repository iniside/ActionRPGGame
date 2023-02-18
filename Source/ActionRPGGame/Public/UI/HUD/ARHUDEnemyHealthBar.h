// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "ARHUDEnemyHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARHUDEnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		class UProgressBar* HealthBar;

public:
	void UpdateHealth(float NormalizedHealth);
};
