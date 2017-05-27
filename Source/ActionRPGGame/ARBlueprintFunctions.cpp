// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "ARBlueprintFunctions.h"




FVector2D UARBlueprintFunctions::CalculateAimOffset(ACharacter* Character, FName Socket, const FVector& HitLocation)
{
	if (!Character)
	{
		return FVector2D();
	}

	const FTransform ActorTransform = Character->GetTransform();
	const USkeletalMeshSocket* LocalSocket = Character->GetMesh()->GetSocketByName(Socket);
	if (LocalSocket)
	{
		const FTransform SocketLocalTransform = LocalSocket->GetSocketLocalTransform();
		const FTransform BoneTransform = Character->GetMesh()->GetSocketTransform(Socket, ERelativeTransformSpace::RTS_Component);// GlobalPose.GetComponentSpaceTransform(SocketBoneIndex);
		const FTransform SocketWorldTransform = SocketLocalTransform * BoneTransform * Character->GetMesh()->GetComponentToWorld();// Component->ComponentToWorld;

		// Convert Target to Actor Space
		const FTransform TargetWorldTransform(HitLocation);

		const FVector DirectionToTarget = ActorTransform.InverseTransformVectorNoScale(TargetWorldTransform.GetLocation() - SocketWorldTransform.GetLocation()).GetSafeNormal();
		const FVector CurrentDirection = ActorTransform.InverseTransformVectorNoScale(SocketWorldTransform.GetUnitAxis(EAxis::X));

		const FVector AxisX = FVector::ForwardVector;
		const FVector AxisY = FVector::RightVector;
		const FVector AxisZ = FVector::UpVector;

		const FVector2D CurrentCoords = FMath::GetAzimuthAndElevation(CurrentDirection, AxisX, AxisY, AxisZ);
		const FVector2D TargetCoords = FMath::GetAzimuthAndElevation(DirectionToTarget, AxisX, AxisY, AxisZ);
		const FVector BlendInput(
			FRotator::NormalizeAxis(FMath::RadiansToDegrees(TargetCoords.X - CurrentCoords.X)),
			FRotator::NormalizeAxis(FMath::RadiansToDegrees(TargetCoords.Y - CurrentCoords.Y)),
			0.f);
		FVector2D ReturnVal(BlendInput.X, BlendInput.Y);

		return ReturnVal;
	}
	return FVector2D();
	// Set X and Y, so ticking next frame is based on correct weights.
	
}