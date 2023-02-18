// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SpectrConsideration.h"
#include "SpectrEvaluator.generated.h"


//USTRUCT(BlueprintType)
//struct SPECTRAI_API FSpectrConsideration
//{
//	GENERATED_BODY()
//public:
//	virtual float Score() const { return 0; }
//	virtual ~FSpectrConsideration()
//	{}
//};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrQualifier
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced)
		TArray<USpectrConsideration*> Considerations;
public:
	virtual ~FSpectrQualifier()
	{}
	virtual float Qualify() const
	{
		float TotalScore = 0;
		for (USpectrConsideration* Consideration : Considerations)
		{
			TotalScore += Consideration->Score();
		}
		return TotalScore;
	}
};

USTRUCT(BlueprintType)
struct SPECTRAI_API FSpectrEvaluator
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FSpectrQualifier Qualifier;

	mutable float Score;

	float Evaluate() const
	{
		Score = 0;
		float score = Qualifier.Qualify();
		Score = score;
		return score;
	}
};

/**
 * 
 */
//UCLASS()
//class SPECTRAI_API USpectrEvaluator : public UObject
//{
//	GENERATED_BODY()
//	
//	
//	
//	
//};
