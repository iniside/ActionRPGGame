// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GAAbilityTask.h"
#include "GAAbilityTask_WaitTargetData.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnTargetingTaskConfimred);

/**
 * 
 */
UCLASS(meta = (ExposedAsyncProxy))
class GAMEABILITIES_API UGAAbilityTask_WaitTargetData : public UGAAbilityTask
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

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGAAbilityTask_WaitTargetData* WaitTargetData(UObject* WorldContextObject, TSubclassOf<class AGATargetingActor> Class, float InRange, 
			ETraceTypeQuery InTraceChannel);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class AGATargetingActor> Class, class AGATargetingActor*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, class AGATargetingActor* SpawnedActor);

	virtual void Activate() override;
	
	virtual void TickTask(float DeltaTime) override;

	virtual void OnDestroy(bool bInOwnerFinished) override;

	UFUNCTION()
		void OnConfirm();

public:
	UGAAbilityTask_WaitTargetData(const FObjectInitializer& ObjectInitializer);
};
