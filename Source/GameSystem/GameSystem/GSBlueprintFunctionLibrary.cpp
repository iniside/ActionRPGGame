// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "EffectField/GSEffectField.h"
#include "Cues/GSPersistentCue.h"

#include "GSBlueprintFunctionLibrary.h"



UGSBlueprintFunctionLibrary::UGSBlueprintFunctionLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


class AGSEffectField* UGSBlueprintFunctionLibrary::BeginCreateEffectField(TSubclassOf<class AGSEffectField> EffectFieldClass
	, const FVector& Location
	, class AActor* FieldInstigator)
{
	AGSEffectField* effectField = nullptr;

	if (FieldInstigator)
	{
		
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation.Pitch = 0;
		//effectField = FieldInstigator->GetWorld()->SpawnActor<AGSEffectField>(EffectFieldClass,
		//	Location, Rotation, FieldInstigator, FieldInstigator->Instigator, true); //change to get pawn
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = FieldInstigator->Instigator;
		FieldInstigator->GetWorld()->SpawnActor<AGSEffectField>(EffectFieldClass, Location, Rotation, SpawnParams);
		if (effectField)
		{
			effectField->EffectFieldInstigator = FieldInstigator;
			effectField->SetActorLocation(Location);
		}
	}

	return effectField;
}


class AGSEffectField* UGSBlueprintFunctionLibrary::FinishCreateEffectField(class AGSEffectField* EffectField)
{
	if (EffectField)
	{
		//FTransform Trans;
		//Trans.SetLocation(EffectField->GetActorLocation());
		//EffectField->FinishSpawning(Trans);
		EffectField->InitializeEffectField();
	}
	return EffectField;
}


class AGSPersistentCue* UGSBlueprintFunctionLibrary::BeginSpawnCueActor(TSubclassOf<class AGSPersistentCue> CueActorClass
	, const FVector& Location
	, class AActor* CueInstigator)
{
	AGSPersistentCue* effectField = nullptr;

	if (CueInstigator)
	{
		FRotator Rotation = FRotator::ZeroRotator;
		Rotation = CueInstigator->GetActorRotation();
		Rotation.Pitch = 0;
		FTransform Trans;
		Trans.SetLocation(Location);
		Trans.SetRotation(FQuat(Rotation));
		effectField = CueInstigator->GetWorld()->SpawnActorDeferred<AGSPersistentCue>(CueActorClass,
			Trans, CueInstigator, CueInstigator->Instigator,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn); //change to get pawn
		if (effectField)
		{
			effectField->CueInstigator = CueInstigator;
		}
	}

	return effectField;
}


class AGSPersistentCue* UGSBlueprintFunctionLibrary::FinishSpawnCueActor(class AGSPersistentCue* EffectField)
{
	if (EffectField)
	{
		FTransform Trans;
		EffectField->FinishSpawning(Trans);
		EffectField->InitializeCue();
	}
	return EffectField;
}