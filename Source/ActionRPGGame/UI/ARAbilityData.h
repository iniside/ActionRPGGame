// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTags.h"
#include "ARAbilityData.generated.h"

USTRUCT()
struct FARAbilityItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UGAAbilityBase> AbilityClass;
	UPROPERTY(EditAnywhere)
		bool bIsAvailable;
 };

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TMap<FGameplayTag, FARAbilityItem> Items;

	UFUNCTION(BlueprintCallable)
		TArray<FGameplayTag> GetAbilitiesTags();
};
