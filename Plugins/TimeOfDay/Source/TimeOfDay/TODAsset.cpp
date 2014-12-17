// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDay.h"
#include "TODAsset.h"
#include "TODActor.h"
UTODAsset::UTODAsset(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//default post process settings:
	AutoExposureMinBrigthness.EditorCurveData.UpdateOrAddKey(0, 0.03);
	AutoExposureMaxBrigthness.EditorCurveData.UpdateOrAddKey(0, 2);
	AutoExposureBias.EditorCurveData.UpdateOrAddKey(0, 0);

	//initialize default atmospheric fog settings:
	SunMultiplier.EditorCurveData.UpdateOrAddKey(0, 1);
	FogMultiplier.EditorCurveData.UpdateOrAddKey(0, 1);
	DensityMultiplier.EditorCurveData.UpdateOrAddKey(0, 1);
	DensityOffset.EditorCurveData.UpdateOrAddKey(0, 0);
	DistanceScale.EditorCurveData.UpdateOrAddKey(0, 1);
	AltitudeScale.EditorCurveData.UpdateOrAddKey(0, 1);
	DistanceOffset.EditorCurveData.UpdateOrAddKey(0, 0);
	GroundOffset.EditorCurveData.UpdateOrAddKey(0, -10000);
	StartDistance.EditorCurveData.UpdateOrAddKey(0, 15000);

}
void UTODAsset::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{

	UProperty* ChangedProperty = PropertyChangedEvent.Property;

	if (ChangedProperty)
	{
		for (TObjectIterator<ATODActor> It; It; ++It)
		{
			if (It->TODAsset == this)
			{
				FTransform trans = FTransform(It->GetActorRotation(), It->GetActorLocation());
				It->OnConstruction(trans);
			}
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}