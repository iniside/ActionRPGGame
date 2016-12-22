// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "GAGlobals.h"
#include "GASAnimNotify.generated.h"

/**
 * 
 */
UCLASS()
class GAMEABILITIES_API UGASAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

		virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere)
		FGASAbilityNotifyData Data;
};
