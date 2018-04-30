// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTags.h"
#include "AFAnimNotify.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName = "Ability Notify"))
class ABILITYFRAMEWORK_API UAFAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	UPROPERTY(EditAnywhere)
		FGameplayTag Tag;
	UPROPERTY(EditAnywhere)
		FName Name;
};
