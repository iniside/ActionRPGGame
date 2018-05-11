// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_TargetData.h"
#include "GAAbilityTask_TargetDataCircle.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASOnReceiveTargetData, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_TargetDataCircle : public UGAAbilityTask_TargetData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnReceiveTargetData OnReceiveTargetDataCircle;

	EGASConfirmType ConfirmType;

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_TargetDataCircle* TargetCircleDataTask(UGAAbilityBase* WorldContextObject,
			FName InTaskName, EGASConfirmType ConfirmTypeIn);

	virtual void Activate() override;

	//UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
	//	bool BeginSpawningActor(UObject* WorldContextObject, class UGASAbilityTargetingObject*& SpawnedActor);

	//UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
	//	void FinishSpawningActor(UObject* WorldContextObject, class UGASAbilityTargetingObject* SpawnedActor);
	//

	virtual void OnConfirm() override;
};
