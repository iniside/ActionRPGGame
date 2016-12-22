// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GAAbilityTask.h"
#include "GAAbilityTask_WaitForConfirm.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnConfirmed);

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGAAbilityTask_WaitForConfirm : public UGAAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnConfirmed OnConfirmed;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGAAbilityTask_WaitForConfirm* CreateWaitConfirmTask(UObject* WorldContextObject);

	virtual void Activate() override;
	virtual void Initialize() override;

	UFUNCTION()
		void OnConfirm();
};
