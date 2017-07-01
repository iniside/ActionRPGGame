// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "GAGlobals.h"
#include "AFAnimNotifyBase.generated.h"

/**
 * 
 */
UCLASS()
class ABILITYFRAMEWORK_API UAFAnimNotifyBase : public UAnimNotify
{
	GENERATED_BODY()

		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
		FAFAbilityNotifyData Data;
	UPROPERTY(EditAnywhere)
		FGameplayTag Tag;
	UPROPERTY(EditAnywhere)
		FName Name;
};
