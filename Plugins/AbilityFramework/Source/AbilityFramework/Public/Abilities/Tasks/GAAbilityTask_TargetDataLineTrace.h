// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GAAbilityTask.h"
#include "Components/SkeletalMeshComponent.h"
#include "GAAbilityTask_TargetDataLineTrace.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAFOnTargetReceived, const FHitResult&, HitResult);

UENUM()
enum class EAFConfirmType : uint8
{
	Instant,
	WaitForConfirm
};

USTRUCT()
struct FAFLineTraceData
{
	GENERATED_BODY()
	UPROPERTY()
		float ExactPing;
	UPROPERTY()
		AActor* HitActor;
	UPROPERTY()
		FVector HitLocation;
};
USTRUCT()
struct FAFLineTraceConfirmData
{
	GENERATED_BODY()
	
	UPROPERTY()
		uint8 bConfirmed : 1;
	UPROPERTY()
		AActor* HitActor;
	UPROPERTY()
		FVector HitLocation;
};
/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UGAAbilityTask_TargetDataLineTrace : public UGAAbilityTask, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
		FAFOnTargetReceived OnClientReceiveTargetData;

	UPROPERTY(BlueprintAssignable)
		FAFOnTargetReceived OnClientUnconfirmedTargetData;

	UPROPERTY(BlueprintAssignable)
		FAFOnTargetReceived OnServerReceiveTargetData;

	ETraceTypeQuery TraceChannel;
	USkeletalMeshComponent* SocketComponent;
	FName SocketName;
	EAFConfirmType ConfirmType;

	float Range;
	bool bIsTickable;
	bool bDrawDebug;

	//Result of trace from either server or client simulation
	FHitResult LocalHitResult;
public:
	UFUNCTION(BlueprintCallable, meta = (HidePin = "WorldContextObject", DefaultToSelf = "WorldContextObject", BlueprintInternalUseOnly = "true"), Category = "AbilityFramework|Abilities|Tasks")
		static UGAAbilityTask_TargetDataLineTrace* CreateTargetDataLineTrace(UGAAbilityBase* WorldContextObject
			, FName InTaskName
			, ETraceTypeQuery InTraceChannel
			, USkeletalMeshComponent* InSocketComponent
			, FName InSocketName
			, bool bDrawDebug
			, EAFConfirmType ConfirmTypeIn
			, float Range);

	UGAAbilityTask_TargetDataLineTrace(const FObjectInitializer& ObjectInitializer);

	virtual void Activate() override;

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerConfirmHitInfo(FAFLineTraceData TraceData);
	void ServerConfirmHitInfo_Implementation(FAFLineTraceData TraceData);
	bool ServerConfirmHitInfo_Validate(FAFLineTraceData TraceData);

	UFUNCTION(Client, Reliable)
		void ClientConfirmHitInfo(FAFLineTraceConfirmData ConfirmData);
	void ClientConfirmHitInfo_Implementation(FAFLineTraceConfirmData ConfirmData);

	UFUNCTION()
		void OnConfirm();

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
