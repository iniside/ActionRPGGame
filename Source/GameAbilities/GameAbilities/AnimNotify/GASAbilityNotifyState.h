// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GAGlobals.h"
#include "GASAbilityNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAbilityNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Data")
		FGASAbilityNotifyData Data;
	UPROPERTY()
	class UGAAbilitiesComponent* CachedAbilitiesComp;
public:
	virtual void NotifyBegin(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float TotalDuration) override;
	virtual void NotifyTick(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(class USkeletalMeshComponent * MeshComp, class UAnimSequenceBase * Animation) override;
	
};
