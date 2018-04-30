// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimationAsset.h"
#include "Animation/AnimNodeBase.h"
#include "AlphaBlend.h"
#include "BoneContainer.h"
#include "OrionInterface.h"
#include "Animation/InputScaleBias.h"
#include "AnimNode_BlendLocomotionFour.generated.h"

class UBlendProfile;
class UCurveFloat;

enum class EECardinalDirection : uint8
{
	N = 0,
	SW = 1,
	E = 2,
	NW = 3,
	S = 4,
	NE = 5,
	W = 6,
	SE = 7
};
USTRUCT(BlueprintType)
struct FOAxisSettings
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = "FAxisSettings")
		TEnumAsByte<EAxis::Type> YawRotationAxis;

	UPROPERTY(EditAnywhere, Category = "FAxisSettings")
		float BodyOrientationAlpha;

	FOAxisSettings() :YawRotationAxis(EAxis::Z)
		, BodyOrientationAlpha(0.5f) {}
};
USTRUCT()
struct FOBoneRef
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = "Settings")
		FBoneReference Bone;
};
// Blend list node; has many children
USTRUCT(BlueprintInternalUseOnly)
struct ORIONANIMATION_API FAnimNode_BlendLocomotionFour : public FAnimNode_Base
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Links)
		TArray<FPoseLink> BlendPose;

	UPROPERTY(EditAnywhere, EditFixedSize, BlueprintReadWrite, Category = Config, meta = (PinShownByDefault))
		TArray<float> BlendTime;

	UPROPERTY(EditAnywhere, Category = BlendType)
		EAlphaBlendOption BlendType;

	UPROPERTY(EditAnywhere, Category = BlendType)
		UCurveFloat* CustomBlendCurve;

	UPROPERTY(EditAnywhere, Category = BlendType)
		UBlendProfile* BlendProfile;

	UPROPERTY()
		TArray<struct FAlphaBlend> Blends;

protected:
	UPROPERTY()
		TArray<float> BlendWeights;

	UPROPERTY()
		TArray<float> RemainingBlendTimes;

	UPROPERTY()
		int32 LastActiveChildIndex;

	UPROPERTY()
		TArray<FBlendSampleData> PerBoneSampleData;

	//Store which poses we need to evaluate
	TArray<int32> PosesToEvaluate;

	/** This reinitializes child pose when re-activated. For example, when active child changes */
	UPROPERTY(EditAnywhere, Category = Option)
		bool bResetChildOnActivation;

protected:
	int32 CurrentDirection;
	EFCardinalDirection OldDirection;
	EFCardinalDirection Dir;
	float OrientN;
	float OrientE;
	float OrientS;
	float OrientW;
	float CurrentOrient;
	bool bDirectionChanged;
	int32 CurrentPose;

	UPROPERTY(Transient)
		mutable float InternalBlendAlpha;

	UPROPERTY(Transient)
		class ACharacter* Character;

	UPROPERTY(Transient)
		class UCharacterMovementComponent* CMC;

	float RemainingBlendTime;
	FAlphaBlend AlphaBlend;

	FAlphaBlend NorthBlend;
	FAlphaBlend EastBlend;

	float DotBlendTime;
	int32 NDot;
	int32 EDot;
public:	
	FAnimNode_BlendLocomotionFour()
		: LastActiveChildIndex(0)
		, bResetChildOnActivation(false)
	{
		BlendTime.SetNum(4);
		BlendPose.SetNum(4);
	}

	// FAnimNode_Base interface
	virtual void Initialize_AnyThread(const FAnimationInitializeContext& Context) override;
	virtual void CacheBones_AnyThread(const FAnimationCacheBonesContext& Context) override;
	virtual void Update_AnyThread(const FAnimationUpdateContext& Context) override;
	virtual void Evaluate_AnyThread(FPoseContext& Output) override;
	virtual void GatherDebugData(FNodeDebugData& DebugData) override;
	// End of FAnimNode_Base interface

protected:
	virtual int32 GetActiveChildIndex() { return 0; }
	virtual FString GetNodeName(FNodeDebugData& DebugData) { return DebugData.GetNodeName(this); }
	void Reorient(FCompactPose& CompactPose, FComponentSpacePoseContext& CSOutput, const FBoneContainer& BoneContainer, float Orient);
};
