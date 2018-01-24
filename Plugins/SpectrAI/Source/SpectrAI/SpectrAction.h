// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "SpectrAction.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SPECTRAI_API USpectrAction : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		int32 Cost;
	/*
		Tag Name of requirent condition in agent state, Desiared State Value
	*/
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		TMap<FGameplayTag, bool> PreConditions;

	/*
		Tag Name of change applied to agent state is effect is executed, Desiared State Value
	*/
	UPROPERTY(EditAnywhere, Category = "State Configuration")
		TMap<FGameplayTag, bool> Effects;

	UFUNCTION(BlueprintNativeEvent)
		void Execute(class USpectrContext* InContext);
	virtual void Execute_Implementation(class USpectrContext* InContext);

	UFUNCTION(BlueprintNativeEvent)
		bool EvaluateCondition(class USpectrContext* InContext);
	virtual bool EvaluateCondition_Implementation(class USpectrContext* InContext);
};
