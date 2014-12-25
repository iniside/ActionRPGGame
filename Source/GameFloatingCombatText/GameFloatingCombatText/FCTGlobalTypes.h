#pragma once
#include "GameplayTagContainer.h"
#include "FCTGlobalTypes.generated.h"

USTRUCT(BlueprintType)
struct FFCTDisplayData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FString DisplayText;

	UPROPERTY(BlueprintReadOnly)
		FVector TargetLocation; //change to vector, we need only position.
	/*
		Might be useful if you want to format widget in specific way depending on 
		what type of data is incoming. Marked as tags.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer Tags;

	float FadeTime;
};

USTRUCT()
struct FFCTDamageHudIndicator
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		float RotationAngle;

	UPROPERTY()
		FString DisplayText;

	UPROPERTY()
		float FadeTime;

	UPROPERTY()
		FVector Location;

	UPROPERTY()
		FVector2D CurrentLocation;

	UPROPERTY()
		FVector2D AnimDirection;

	FFCTDamageHudIndicator()
		: RotationAngle(0.0f)
		, DisplayText("")
		, FadeTime(0.0f)
		, Location(FVector::ZeroVector)
	{
	}
};

USTRUCT(BlueprintType)
struct FFCTDisplaySettings
{
	GENERATED_USTRUCT_BODY();
public:
	UPROPERTY(EditAnywhere, Category = "FCT")
		float FontScale;
	UPROPERTY(EditAnywhere, Category = "FCT")
		FLinearColor FontColor;
	UPROPERTY(EditAnywhere, Category = "FCT")
		UFont* FontType;

	UPROPERTY(EditAnywhere, Category = "FCT")
		float AnimationXMin;
	UPROPERTY(EditAnywhere, Category = "FCT")
		float AnimationXMax;

	UPROPERTY(EditAnywhere, Category = "FCT")
		float AnimationYMin;
	UPROPERTY(EditAnywhere, Category = "FCT")
		float AnimationYMax;

	UPROPERTY(EditAnywhere, Category = "FCT")
		float FCTLifeTime;

	UPROPERTY(EditAnywhere, Category = "FCT")
		float RandomOffset;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FFCTOnReceivedData, const FFCTDisplayData&);