// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SpectrAction.h"
#include "STestAction_ChopFirewood.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class SPECTRAITEST_API USTestAction_ChopFirewood : public USpectrAction
{
	GENERATED_BODY()

		UPROPERTY()
		AActor* TargetTree;
	UPROPERTY(EditAnywhere)
		float MinDistance;

public:
	virtual bool NativeIsInRange(class AAIController* AIController) override;
	virtual void NativeMoveTo(class USpectrBrainComponent* Brain) override;
	virtual void NativeOnMoveFinished(class USpectrContext* InContext
		, class AAIController* AIController
		, class USpectrBrainComponent* Brain) override;

	//virtual void NativeExecute(class USpectrContext* InContext
	//	, class AAIController* AIController
	//	, class USpectrBrainComponent* Brain) override;

	virtual bool NativeEvaluateCondition(class USpectrContext* InContext, class AAIController* AIController);
	
	
};
