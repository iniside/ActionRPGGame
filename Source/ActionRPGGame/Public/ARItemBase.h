// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFItemBase.h"
#include "Effects/GAGameEffect.h"
#include "ARItemBase.generated.h"


USTRUCT(BlueprintType)
struct FARPlayerAttributeMod
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Mod")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, Category = "Mod")
		float Value;
	
	UPROPERTY(EditAnywhere, Category = "Mod")
		EGAAttributeMod ModType;
};

USTRUCT(BlueprintType)
struct FARItemTooltipData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FString Key;
	UPROPERTY(BlueprintReadOnly)
		FString Value;

	FARItemTooltipData()
	{};

	FARItemTooltipData(const FString& InKey, const FString InValue)
		: Key(InKey)
		, Value(InValue)
	{};
};

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARItemBase : public UIFItemBase
{
	GENERATED_BODY()
public:

	/*
		Perks (Effects with extensions), to apply to owner of this item.
	*/
	UPROPERTY(EditAnywhere, Category = "Apply To Player")
		TArray<TSubclassOf<UAFEffectSpecBase>> EffectsToApply;

	/*
		Attribute modifying effects which will be applied to player.
	*/
	UPROPERTY(EditAnywhere, Category = "Apply To Player")
		TArray<FARPlayerAttributeMod> AttributesModifiers;
	/*
		An Template from which attribute effects will be created.
		Values from AttributesModifiers property will be used.
	*/
	UPROPERTY(EditAnywhere, Category = "Apply To Player")
		TSubclassOf<UAFEffectSpecBase> AttributeEffectClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual")
		UTexture2D* Icon;
	/*
		Containes upgrades to THIS item.
	*/
	UPROPERTY(Transient)
		FAFEffectContainerSimple UpgradeContainer;

	virtual TArray<FARItemTooltipData> GetTooltipData()
	{
		TArray<FARItemTooltipData> Data;
		return Data;
	}
};
