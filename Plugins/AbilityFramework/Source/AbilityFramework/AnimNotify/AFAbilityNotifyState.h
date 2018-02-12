// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTags.h"
#include "AFAbilityNotifyState.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Ability Notify State"))
class ABILITYFRAMEWORK_API UAFAbilityNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		FGameplayTag Tag;
	UPROPERTY(EditAnywhere)
		FName Name;
	UPROPERTY()
	class UAFAbilityComponent* CachedAbilitiesComp;
public:
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;
	
};
