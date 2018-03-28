// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ARUMGWidgetBase.h"
#include "GameplayTags.h"
#include "Abilities/GAAbilityBase.h"
#include "ARAbilityInfoWidget.h"
#include "ARAbilityCrosshairInfo.generated.h"
/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityCrosshairInfo : public UARAbilityInfoWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Config")
		UMaterialInterface* CrosshairMaterial;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UImage* CrosshairImage;
public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "ActionRPGGame|UI")
		void OnAbilityGroupChanged(EAMGroup CurrentGroup);
};
