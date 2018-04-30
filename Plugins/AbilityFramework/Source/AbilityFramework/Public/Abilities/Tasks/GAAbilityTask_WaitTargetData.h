// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_WaitTargetData.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnTargetingTaskConfimred);

/**
 * 
 */
UCLASS(meta = (ExposedAsyncProxy))
class ABILITYFRAMEWORK_API UGAAbilityTask_WaitTargetData : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnTargetingTaskConfimred OnConfirmed;

	UPROPERTY()
		float Range;
	ETraceTypeQuery TraceChannel;

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction) override;


	UFUNCTION()
		void OnConfirm();

public:
	UGAAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer);
};
