// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemBase.h"
#include "Effects/GAGameEffect.h"
#include "ARItemBase.generated.h"

USTRUCT()
struct FARItemBaseData : public FIFItemBaseData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
		TSoftObjectPtr<UTexture2D> Icon;
};
/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemBase : public UIFItemBase
{
	GENERATED_BODY()
public:
	//obviously we want TSoftObjectPtr<>
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
		UTexture2D* Icon;
	/*
		Containes upgrades to THIS item.
	*/
	UPROPERTY(Transient)
		FAFEffectContainerSimple UpgradeContainer;
};
