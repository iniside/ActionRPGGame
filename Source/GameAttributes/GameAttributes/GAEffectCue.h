// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GAGlobalTypes.h"
#include "GAEffectCue.generated.h"

/**
 * Base class containing information about Cue.
 * Cue is visual/sound representation of effect in the world.
 * This is only for testing. Do not use it in the current state!
 */
UCLASS()
class GAMEATTRIBUTES_API AGAEffectCue : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		bool bAttachToOwner;

	UPROPERTY(BlueprintReadOnly, Category = "Data")
	FGAEffectCueParams CueParams;

	virtual void Destroyed() override;
	/*
		These events mirrors the onces in effect, so you can implement different visuals, for
		each state, effect can be at time.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectApplied();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectPeriod();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectEnded();

	UFUNCTION(BlueprintImplementableEvent, Category = "Effect Cue")
		void OnEffectRemoved();

	AGAEffectCue(const FObjectInitializer& ObjectInitializer);
};
