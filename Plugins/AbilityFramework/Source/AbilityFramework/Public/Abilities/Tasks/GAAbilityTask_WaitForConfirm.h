// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_WaitForConfirm.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnConfirmed);

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_WaitForConfirm : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnConfirmed OnConfirmed;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_WaitForConfirm* CreateWaitConfirmTask(UGAAbilityBase* WorldContextObject,
			FName InTaskName);

	virtual void Activate() override;
	virtual void Initialize() override;

	UFUNCTION()
		void OnConfirm();
};
