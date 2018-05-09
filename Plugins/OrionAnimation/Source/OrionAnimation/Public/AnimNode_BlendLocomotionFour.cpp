// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "AnimNode_BlendLocomotionFour.h"
#include "AnimationRuntime.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Animation/AnimClassInterface.h"
#include "Animation/AnimInstanceProxy.h"
#include "Animation/BlendProfile.h"
#include "DrawDebugHelpers.h"

#include "Engine.h"
/////////////////////////////////////////////////////
// FAnimNode_BlendLocomotionFour
FString FDirToString(EFCardinalDirection Dir)
{
	switch (Dir)
	{
	case EFCardinalDirection::N:
		return "N";
	case EFCardinalDirection::E:
		return "E";
	case EFCardinalDirection::S:
		return "S";
	case EFCardinalDirection::W:
		return "W";
	default:
		break;
	}
	return "Invalid";
}
void FAnimNode_BlendLocomotionFour::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	FAnimNode_Base::Initialize_AnyThread(Context);
	
	UAnimInstance* AnimInst = Cast<UAnimInstance>(Context.AnimInstanceProxy->GetAnimInstanceObject());
	Character = Cast<ACharacter>(AnimInst->TryGetPawnOwner());
	if (!Character)
		return;

	CMC = Character->GetCharacterMovement();

	if (!CMC)
		return;

	const int NumPoses = BlendPose.Num();
	checkSlow(BlendTime.Num() == NumPoses);

	BlendWeights.Reset(NumPoses);
	PosesToEvaluate.Reset(NumPoses);
	if (NumPoses > 0)
	{
		// If we have at least 1 pose we initialize to full weight on
		// the first pose
		BlendWeights.AddZeroed(NumPoses);
		BlendWeights[0] = 1.0f;

		PosesToEvaluate.Add(0);

		for (int32 ChildIndex = 0; ChildIndex < NumPoses; ++ChildIndex)
		{
			BlendPose[ChildIndex].Initialize(Context);
		}
	}

	RemainingBlendTimes.Empty(NumPoses);
	RemainingBlendTimes.AddZeroed(NumPoses);
	Blends.Empty(NumPoses);
	Blends.AddZeroed(NumPoses);

	LastActiveChildIndex = INDEX_NONE;

	for (int32 i = 0; i < Blends.Num(); ++i)
	{
		FAlphaBlend& Blend = Blends[i];

		Blend.SetBlendTime(0.0f);
		Blend.SetBlendOption(BlendType);
		Blend.SetCustomCurve(CustomBlendCurve);
	}
	Blends[0].SetAlpha(1.0f);

	if (BlendProfile)
	{
		// Initialise per-bone data
		PerBoneSampleData.Empty(NumPoses);
		PerBoneSampleData.AddZeroed(NumPoses);

		for (int32 Idx = 0; Idx < NumPoses; ++Idx)
		{
			FBlendSampleData& SampleData = PerBoneSampleData[Idx];
			SampleData.SampleDataIndex = Idx;
			SampleData.PerBoneBlendData.AddZeroed(BlendProfile->GetNumBlendEntries());
		}
	}
}

void FAnimNode_BlendLocomotionFour::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	for (int32 ChildIndex = 0; ChildIndex<BlendPose.Num(); ChildIndex++)
	{
		BlendPose[ChildIndex].CacheBones(Context);
	}
}
void FAnimNode_BlendLocomotionFour::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	EvaluateGraphExposedInputs.Execute(Context);

	if (!Character || !CMC)
		return;

	FVector Right = Character->GetActorRightVector();
	FVector Forward = Character->GetActorForwardVector();

	FTransform Transform = Character->GetTransform();
	FVector CurrentAcceleration = CMC->GetCurrentAcceleration();
	FVector CurrentVelocity = CMC->Velocity;

	FVector AccelerationDirection = CurrentAcceleration.GetSafeNormal2D();
	FVector VelocityDirection = CurrentVelocity.GetSafeNormal2D();

	FVector LocalAcceleration = Transform.InverseTransformVectorNoScale(AccelerationDirection);
	FVector LocalVelocity = Transform.InverseTransformVectorNoScale(VelocityDirection);
	
	float Atan2Angle = FMath::Atan2(LocalVelocity.Y, LocalVelocity.X);
	int32 Dir = FMath::RoundToInt((Atan2Angle * 2 / PI) + 4) % 4;
	DotBlendTime = FMath::Abs(FVector::DotProduct(LocalVelocity, LocalAcceleration));
	BlendTime[0] = DotBlendTime;
	BlendTime[1] = DotBlendTime;
	BlendTime[2] = DotBlendTime;
	BlendTime[3] = DotBlendTime;

	NDot = FMath::RoundToInt(FVector::DotProduct(Forward, VelocityDirection));
	EDot = FMath::RoundToInt(FVector::DotProduct(Right, VelocityDirection));

	const int NumPoses = BlendPose.Num();
	checkSlow((BlendTime.Num() == NumPoses) && (BlendWeights.Num() == NumPoses));

	PosesToEvaluate.Empty(NumPoses);

	if (NumPoses > 0)
	{
		// Handle a change in the active child index; adjusting the target weights
		const int32 ChildIndex = FMath::RoundToInt((Atan2Angle * 2 / PI) + 4) % 4;;

		if (ChildIndex != LastActiveChildIndex)
		{
			bool LastChildIndexIsInvalid = (LastActiveChildIndex == INDEX_NONE);

			const float CurrentWeight = BlendWeights[ChildIndex];
			const float DesiredWeight = 1.0f;
			const float WeightDifference = FMath::Clamp<float>(FMath::Abs<float>(DesiredWeight - CurrentWeight), 0.0f, 1.0f);

			// scale by the weight difference since we want always consistency:
			// - if you're moving from 0 to full weight 1, it will use the normal blend time
			// - if you're moving from 0.5 to full weight 1, it will get there in half the time
			const float RemainingBlendTime = LastChildIndexIsInvalid ? 0.0f : (BlendTime[ChildIndex] * WeightDifference);

			for (int32 i = 0; i < RemainingBlendTimes.Num(); ++i)
			{
				RemainingBlendTimes[i] = RemainingBlendTime;
			}

			// If we have a valid previous child and we're instantly blending - update that pose with zero weight
			if (RemainingBlendTime == 0.0f && !LastChildIndexIsInvalid)
			{
				BlendPose[LastActiveChildIndex].Update(Context.FractionalWeight(0.0f));
			}

			for (int32 i = 0; i < Blends.Num(); ++i)
			{
				FAlphaBlend& Blend = Blends[i];

				Blend.SetBlendTime(RemainingBlendTime);

				if (i == ChildIndex)
				{
					Blend.SetValueRange(BlendWeights[i], 1.0f);
				}
				else
				{
					Blend.SetValueRange(BlendWeights[i], 0.0f);
				}
			}

			// when this flag is true, we'll reinitialize the children
			if (bResetChildOnActivation)
			{
				FAnimationInitializeContext ReinitializeContext(Context.AnimInstanceProxy);

				// reinitialize
				BlendPose[ChildIndex].Initialize(ReinitializeContext);
			}

			LastActiveChildIndex = ChildIndex;
		}

		// Advance the weights
		//@TODO: This means we advance even in a frame where the target weights/times just got modified; is that desirable?
		float SumWeight = 0.0f;
		for (int32 i = 0; i < Blends.Num(); ++i)
		{
			float& BlendWeight = BlendWeights[i];

			FAlphaBlend& Blend = Blends[i];
			Blend.Update(Context.GetDeltaTime());
			BlendWeight = Blend.GetBlendedValue();

			SumWeight += BlendWeight;
		}

		// Renormalize the weights
		if ((SumWeight > ZERO_ANIMWEIGHT_THRESH) && (FMath::Abs<float>(SumWeight - 1.0f) > ZERO_ANIMWEIGHT_THRESH))
		{
			float ReciprocalSum = 1.0f / SumWeight;
			for (int32 i = 0; i < BlendWeights.Num(); ++i)
			{
				BlendWeights[i] *= ReciprocalSum;
			}
		}

		// Update our active children
		for (int32 i = 0; i < BlendPose.Num(); ++i)
		{
			const float BlendWeight = BlendWeights[i];
			if (BlendWeight > ZERO_ANIMWEIGHT_THRESH)
			{
				BlendPose[i].Update(Context.FractionalWeight(BlendWeight));
				PosesToEvaluate.Add(i);
			}
		}

		// If we're using a blend profile, extract the scales and build blend sample data
		if (BlendProfile)
		{
			for (int32 i = 0; i < BlendPose.Num(); ++i)
			{
				// Update Per-Bone Info
				const float BlendWeight = BlendWeights[i];
				FBlendSampleData& PoseSampleData = PerBoneSampleData[i];
				PoseSampleData.TotalWeight = BlendWeight;

				for (int32 j = 0; j < PoseSampleData.PerBoneBlendData.Num(); ++j)
				{
					float& BoneBlend = PoseSampleData.PerBoneBlendData[j];
					float WeightScale = BlendProfile->GetEntryBlendScale(j);

					if (ChildIndex != i)
					{
						WeightScale = 1.0f / WeightScale;
					}

					BoneBlend = BlendWeight * WeightScale;
				}
			}

			FBlendSampleData::NormalizeDataWeight(PerBoneSampleData);
		}
	}
	

	switch (static_cast<EFCardinalDirection>(Dir))
	{
	case EFCardinalDirection::N:
	{
		FQuat ForwardQuat = FQuat::FindBetween(Forward, CurrentVelocity);
		OrientN = FRotator(ForwardQuat).Yaw;
		CurrentOrient = OrientN;// 
		//if (NDot == EDot)
		/*{
			NorthPose.Update(Context.FractionalWeight(DotBlendTime));
			EastPose.Update(Context.FractionalWeight(1.0f - DotBlendTime));
		}*/
		/*else
		{
			NorthPose.Update(Context);
		}*/
		
		break;
	}
	case EFCardinalDirection::E:
	{
		FQuat LeftQuat = FQuat::FindBetween(Right, CurrentVelocity);
		OrientE = FRotator(LeftQuat).Yaw;
		CurrentOrient = OrientE;
		//if (NDot == EDot)
		/*{
			NorthPose.Update(Context.FractionalWeight(1.0f - DotBlendTime));
			EastPose.Update(Context.FractionalWeight(DotBlendTime));
		}*/
		/*else
		{
			EastPose.Update(Context);
		}*/
		
		// FMath::FInterpConstantTo(OldOrient, OrientE, DeltaTime, 300.0f);
		break;
	}
	case EFCardinalDirection::S:
	{
		FQuat BackQuat = FQuat::FindBetween(Forward*(-1), CurrentVelocity);
		OrientS = FRotator(BackQuat).Yaw;
		CurrentOrient = OrientS;
		break;
	}
	case EFCardinalDirection::W:
	{
		FQuat RightQuat = FQuat::FindBetween(Right*(-1), CurrentVelocity);
		OrientW = FRotator(RightQuat).Yaw;
		CurrentOrient = OrientW;// FMath::FInterpConstantTo(OldOrient, OrientW, DeltaTime, 300.0f);
		break;
	}
	default:
		break;
	}

}

void FAnimNode_BlendLocomotionFour::Evaluate_AnyThread(FPoseContext& Output)
{
	ANIM_MT_SCOPE_CYCLE_COUNTER(BlendPosesInGraph, !IsInGameThread());
	{
		//BlendPose[CurrentPose].Evaluate(Output);
	}
	const int32 NumPoses = PosesToEvaluate.Num();
	if ((NDot == EDot) && (NumPoses > 0) && (BlendPose.Num() == BlendWeights.Num()))
	{
		// Scratch arrays for evaluation, stack allocated
		TArray<FCompactPose, TInlineAllocator<8>> FilteredPoses;
		TArray<FBlendedCurve, TInlineAllocator<8>> FilteredCurve;
		FilteredPoses.SetNum(NumPoses, false);
		FilteredCurve.SetNum(NumPoses, false);

		int32 NumActivePoses = 0;
		for (int32 i = 0; i < PosesToEvaluate.Num(); ++i)
		{
			int32 PoseIndex = PosesToEvaluate[i];

			FPoseContext EvaluateContext(Output);

			FPoseLink& CurrentPose = BlendPose[PoseIndex];
			CurrentPose.Evaluate(EvaluateContext);

			FilteredPoses[i].CopyBonesFrom(EvaluateContext.Pose);
			FilteredCurve[i] = EvaluateContext.Curve;
		}

		// Use the calculated blend sample data if we're blending per-bone
		if (BlendProfile)
		{
			FAnimationRuntime::BlendPosesTogetherPerBone(FilteredPoses, FilteredCurve, BlendProfile, PerBoneSampleData, PosesToEvaluate, Output.Pose, Output.Curve);
		}
		else
		{
			FAnimationRuntime::BlendPosesTogether(FilteredPoses, FilteredCurve, BlendWeights, PosesToEvaluate, Output.Pose, Output.Curve);
		}
		
	}
	else
	{
		for (int32 i = 0; i < PosesToEvaluate.Num(); ++i)
		{
			int32 PoseIndex = PosesToEvaluate[i];

			FPoseLink& CurrentPose = BlendPose[PoseIndex];
			CurrentPose.Evaluate(Output);

		}
		//Output.ResetToRefPose();
	}

	//FPoseContext EvaluateContext(Output);
	//const FBoneContainer& BoneContainer = Output.AnimInstanceProxy->GetRequiredBones();
	//FComponentSpacePoseContext CSOutput(Output.AnimInstanceProxy);
	//EFCardinalDirection ghf = Cast<IOrionInterface>(Character)->GetCardianlDirection();
	//switch (Dir)
	//{
	//case EFCardinalDirection::N:
	//{
	//	//if (OldDirection != Dir)
	//	{
	//		//if (NDot == EDot)
	//		{
	//			//Reorient(Output.Pose, CSOutput, BoneContainer, OrientN);

	//			FPoseContext Pose1(Output);
	//			FPoseContext Pose2(Output);

	//			NorthPose.Evaluate(Pose1);
	//			EastPose.Evaluate(Pose2);
	//			FAnimationRuntime::BlendTwoPosesTogether(Pose1.Pose, Pose2.Pose, Pose1.Curve, Pose2.Curve, (DotBlendTime), Output.Pose, Output.Curve);
	//			
	//		}
	//		/*else
	//		{
	//			NorthPose.Evaluate(Output);
	//			Reorient(Output.Pose, CSOutput, BoneContainer, OrientN);
	//		}*/
	//	}
	//	/*else
	//	{
	//		NorthPose.Evaluate(Output);
	//		Reorient(Output.Pose, CSOutput, BoneContainer, OrientN);
	//	}*/
	//	break;
	//}
	//case EFCardinalDirection::E:
	//{
	//	
	//	//if (OldDirection != Dir)
	//	{
	//		//if (NDot == EDot)
	//		{
	//			//Reorient(Output.Pose, CSOutput, BoneContainer, OrientE);

	//			FPoseContext Pose1(Output);
	//			FPoseContext Pose2(Output);

	//			NorthPose.Evaluate(Pose2);
	//			EastPose.Evaluate(Pose1);
	//			
	//			FAnimationRuntime::BlendTwoPosesTogether(Pose1.Pose, Pose2.Pose, Pose1.Curve, Pose2.Curve, (DotBlendTime), Output.Pose, Output.Curve);
	//			
	//			
	//		}
	//		/*else
	//		{
	//			EastPose.Evaluate(Output);
	//			Reorient(Output.Pose, CSOutput, BoneContainer, OrientE);
	//		}*/

	//	}
	//	/*else
	//	{
	//		EastPose.Evaluate(Output);
	//		Reorient(Output.Pose, CSOutput, BoneContainer, OrientE);
	//	}*/
	//	break;
	//}
	//case EFCardinalDirection::S:
	//{
	//	break;
	//}
	//case EFCardinalDirection::W:
	//{
	//	break;
	//}
	//{
	//default:
	//	break;
	//}
	//}

}

void FAnimNode_BlendLocomotionFour::GatherDebugData(FNodeDebugData& DebugData)
{
	//const int32 ChildIndex = GetActiveChildIndex();

	//FString DebugLine = GetNodeName(DebugData);
	//DebugLine += FString::Printf(TEXT("(BlendTime: %f "), DotBlendTime);

	//DebugData.AddDebugItem(DebugLine);

	//NorthPose.GatherDebugData(DebugData.BranchFlow(DotBlendTime));
	//EastPose.GatherDebugData(DebugData.BranchFlow(DotBlendTime));
}


void FAnimNode_BlendLocomotionFour::Reorient(FCompactPose& CompactPose, FComponentSpacePoseContext& CSOutput, const FBoneContainer& BoneContainer, float Orient)
{
	if (!FMath::IsNearlyZero(Orient, KINDA_SMALL_NUMBER))
	{
		const FRotator DeltaRotation(0.0f, Orient, 0.f);
		const FQuat DeltaQuat(DeltaRotation);
		const FQuat MeshToComponentQuat(FRotator::ZeroRotator);

		// Convert our rotation from Component Space to Mesh Space.
		const FQuat MeshSpaceDeltaQuat = MeshToComponentQuat.Inverse() * DeltaQuat * MeshToComponentQuat;

		// Apply rotation to root bone.
		FCompactPoseBoneIndex RootBoneIndex(0);
		CompactPose[RootBoneIndex].SetRotation(CompactPose[RootBoneIndex].GetRotation() * DeltaQuat);
		CompactPose[RootBoneIndex].NormalizeRotation();
	}
	//	if (IKFootRootBone.IsValidToEvaluate(BoneContainer))
	//	{
	//		// Prepare convert Quat and BoneContainer.
	//		const FQuat MeshToComponentQuat(FRotator(0.f, 0.f, 0.f));

	//		CSOutput.Pose.InitPose(CompactPose);

	//		//Build our desired rotation for IK root bone.
	//		FRotator DeltaRotation(0.0f, 0.0f, 0.f);
	//		switch (Settings.YawRotationAxis)
	//		{
	//		case EAxis::X:
	//			DeltaRotation.Roll = Orient;
	//		case EAxis::Y:
	//			DeltaRotation.Pitch = Orient;
	//		case EAxis::Z:
	//			DeltaRotation.Yaw = Orient;
	//		default:
	//			break;
	//		}
	//		const FQuat DeltaQuat(DeltaRotation);
	//		// Convert our rotation from Component Space to Mesh Space.
	//		const FQuat MeshSpaceDeltaQuat = DeltaQuat;
	//		// Apply rotation to IK root bone.
	//		FCompactPoseBoneIndex RotateBoneIndex = IKFootRootBone.GetCompactPoseIndex(BoneContainer);
	//		CompactPose[RotateBoneIndex].SetRotation(CompactPose[RotateBoneIndex].GetRotation() * MeshSpaceDeltaQuat);
	//		CompactPose[RotateBoneIndex].NormalizeRotation();


	//		// Do the same things like IK foot root bone to pelvis, but in the reversed orientation.
	//		FCompactPoseBoneIndex PelvisBoneIndex(1);
	//		FTransform PelvisBoneTM = CSOutput.Pose.GetComponentSpaceTransform(PelvisBoneIndex);

	//		const FRotator PelvisDeltaRotation(DeltaRotation.Pitch * Settings.BodyOrientationAlpha, DeltaRotation.Yaw * Settings.BodyOrientationAlpha, DeltaRotation.Roll * Settings.BodyOrientationAlpha);
	//		FQuat PelvisDeltaQuat(PelvisDeltaRotation);

	//		//const FQuat MeshSpacePelvisDeltaQuat =  PelvisBoneTM.GetRotation().Inverse() *PelvisDeltaQuat * PelvisBoneTM.GetRotation();
	//		FQuat MeshSpacePelvisDeltaQuat = PelvisBoneTM.GetRotation().Inverse()*PelvisDeltaQuat * PelvisBoneTM.GetRotation();

	//		CompactPose[PelvisBoneIndex].ConcatenateRotation(MeshSpacePelvisDeltaQuat);
	//		CompactPose[PelvisBoneIndex].NormalizeRotation();

	//		// Apply rotation to spine
	//		if (SpineBones.Num())
	//		{
	//			for (int32 j = 0; j < SpineBones.Num(); j++)
	//			{
	//				if (SpineBones[j].Bone.IsValidToEvaluate(BoneContainer))
	//				{
	//					FCompactPoseBoneIndex SpineBoneIndex = SpineBones[j].Bone.GetCompactPoseIndex(BoneContainer);
	//					FTransform SpineBoneTM = CSOutput.Pose.GetComponentSpaceTransform(SpineBoneIndex);
	//					const FRotator SpineDeltaRotation((-PelvisDeltaRotation.Pitch / SpineBones.Num()), (-PelvisDeltaRotation.Yaw / SpineBones.Num()), (-PelvisDeltaRotation.Roll / SpineBones.Num()));
	//					const FQuat SpineDeltaQuat(SpineDeltaRotation);
	//					const FQuat MeshSpaceSpineDeltaQuat = SpineBoneTM.GetRotation().Inverse() * SpineDeltaQuat * SpineBoneTM.GetRotation();
	//					CompactPose[SpineBoneIndex].ConcatenateRotation(MeshSpaceSpineDeltaQuat);
	//					CompactPose[SpineBoneIndex].NormalizeRotation();
	//					//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Yellow, (FString::Printf(TEXT(" SpineDeltaQuat: %s, MeshSpaceSpineDeltaQuat: %s PelvisDeltaQuat: %s"), *SpineDeltaQuat.ToString(), *MeshSpaceSpineDeltaQuat.ToString(), *PelvisDeltaQuat.ToString())));
	//				}
	//			}
	//		}
	//	}
	//}
}
