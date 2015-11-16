// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_WaitForConfirm.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASOnConfirmed);

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_WaitForConfirm : public UGASAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnConfirmed OnConfirmed;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_WaitForConfirm* CreateWaitConfirmTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityMod> Class, UObject* Outer);

	virtual void Initialize() override;

	UFUNCTION()
		void OnConfirm();
};
