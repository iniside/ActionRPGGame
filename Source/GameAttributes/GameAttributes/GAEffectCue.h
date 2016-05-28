// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GAGlobalTypes.h"
#include "GAEffectCue.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew)
class GAMEATTRIBUTES_API UGAEffectCue : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Tick")
		bool bEnableTick;

	UPROPERTY(BlueprintReadOnly, Category = "Cue")
		AActor* Owner;
	
	UPROPERTY(BlueprintReadOnly, Category = "Cue")
		FGAEffectCueParams CueParams;

	bool bEffectRemoved;
	bool bTickEnabled;
protected:
	FTimerHandle DurationTimerHandle;
	FTimerHandle PeriodTimerHandle;
public:
	UGAEffectCue();
	virtual void SetParameters(AActor* OwnerIn, const FGAEffectCueParams& CueParamsIn);
	virtual void BeginCue();
	void ExecuteCue();
	void UpdateDuration(float NewDurationIn);
	void ExtendDuration(float Duration);
	void UpdatePeriod(float NewPeriodIn);
	void UpdateTimers(float NewDurationIn, float NewPeriodIn);
	void EndCue();
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectApplied();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectPeriod();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectRemoved();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnTick();

	/* FTickableGameObject Implementation Begin */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	/* FTickableGameObject Implementation End */

	virtual UWorld* GetWorld() const override;
	
};
