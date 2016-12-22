// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GAAbilityTask.h"
#include "GAAbilityTask_TargetData.h"
#include "GAAbilityTask_TargetDataCircle.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASOnReceiveTargetData, const FHitResult&, HitResult);

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGAAbilityTask_TargetDataCircle : public UGAAbilityTask_TargetData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnReceiveTargetData OnReceiveTargetDataCircle;

	UPROPERTY()
	class UGASAbilityTargetingObject* TargetObj2;

	EGASConfirmType ConfirmType;

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGAAbilityTask_TargetDataCircle* TargetCircleDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityTargetingObject> Class, EGASConfirmType ConfirmTypeIn);

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class UGASAbilityTargetingObject> Class, class UGASAbilityTargetingObject*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, class UGASAbilityTargetingObject* SpawnedActor);
	//

	UFUNCTION()
		void OnConfirm();
};
