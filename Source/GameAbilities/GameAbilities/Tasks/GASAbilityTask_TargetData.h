// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Tasks/GASAbilityTask.h"
#include "GASAbilityTask_TargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASOnReceiveTargetData, const FHitResult&, HitResult);

UENUM()
enum class EGASConfirmType : uint8
{
	Instant,
	WaitForPlayer
};


/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityTask_TargetData : public UGASAbilityTask
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnReceiveTargetData OnReceiveTargetData;

	UPROPERTY()
	class UGASAbilityTargetingObject* TargetObj;

	EGASConfirmType ConfirmType;

public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities | Tasks")
		static UGASAbilityTask_TargetData* CreateTargetDataTask(UObject* WorldContextObject, TSubclassOf<class UGASAbilityTargetingObject> Class, EGASConfirmType ConfirmTypeIn);

	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		bool BeginSpawningActor(UObject* WorldContextObject, TSubclassOf<class UGASAbilityTargetingObject> Class, class UGASAbilityTargetingObject*& SpawnedActor);

	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "Game Abilities")
		void FinishSpawningActor(UObject* WorldContextObject, class UGASAbilityTargetingObject* SpawnedActor);
	//

	UFUNCTION()
		void OnConfirm();
};
