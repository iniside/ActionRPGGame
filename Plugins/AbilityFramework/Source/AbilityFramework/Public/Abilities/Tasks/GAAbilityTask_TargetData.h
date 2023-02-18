// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "GAAbilityTask_TargetData.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGASOnReceiveTargetData, const FHitResult&, HitResult);

UENUM()
enum class EGASConfirmType : uint8
{
	Instant,
	WaitForConfirm
};


/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_TargetData : public UGAAbilityTask, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FGASOnReceiveTargetData OnConfirmed;
	UPROPERTY(BlueprintAssignable)
		FGASOnReceiveTargetData OnReceiveTargetData;
	
	EGASConfirmType ConfirmType;

	float Range;
	bool bIsTickable;
	bool bDrawDebug;
	bool bDrawCorrectedDebug;
	bool bUseCorrectedTrace;
public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_TargetData* CreateTargetDataTask(UGAAbilityBase* WorldContextObject,
			FName InTaskName,
			bool bDrawDebug,
			bool bDrawCorrectedDebug,
			bool bUseCorrectedTrace,
			EGASConfirmType ConfirmTypeIn,
			float Range);

	virtual void Activate() override;

	UFUNCTION()
		virtual void OnConfirm();

	UFUNCTION()
		void OnCastEndedConfirm();

	/* FTickableGameObject Begin */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override { return bIsTickable; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UGAAbilityTask_TargetData, STATGROUP_Tickables); };
	/* FTickableGameObject End */

protected:
	FHitResult LineTrace();
};
