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
	UPROPERTY()
		class AGATargetingActor* TargetActor;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_WaitTargetData* WaitTargetData(UObject* WorldContextObject, 
			FName InTaskName, TSubclassOf<class AGATargetingActor> Class, float InRange, ETraceTypeQuery InTraceChannel);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class AGATargetingActor> Class, class AGATargetingActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		void FinishSpawningActor(UObject* WorldContextObject, class AGATargetingActor* SpawnedActor);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
		void OnConfirm();

public:
	UGAAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer);
};
