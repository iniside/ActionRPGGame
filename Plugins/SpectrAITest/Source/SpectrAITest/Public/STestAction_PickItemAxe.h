// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpectrAction.h"
#include "STestAction_PickItemAxe.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SPECTRAITEST_API USTestAction_PickItemAxe : public USpectrAction
{
	GENERATED_BODY()
protected:
	/* Minimum distance from item to pick it. */
	UPROPERTY(EditAnywhere)
		float MinDistance;
	UPROPERTY()
		AActor* TargetItem;
public:
	virtual bool NativeIsInRange(class AAIController* AIController) override;
	virtual void NativeMoveTo(class USpectrBrainComponent* Brain) override;
	virtual void NativeOnMoveFinished(class USpectrContext* InContext
		, class AAIController* AIController
		, class USpectrBrainComponent* Brain) override;
	
	virtual void NativeExecute(class USpectrContext* InContext
		, class AAIController* AIController
		, class USpectrBrainComponent* Brain) override;
	
		virtual bool NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController);
};
