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
		Rotation = FieldInstigator->GetActorRotation();
		Rotation.Pitch = 0;
		effectField = FieldInstigator->GetWorld()->SpawnActorDeferred<AGSEffectField>(EffectFieldClass,
			Location, Rotation, FieldInstigator, nullptr, true); //change to get pawn
		if (effectField)
		{
			effectField->EffectFieldInstigator = FieldInstigator;
		}
	}

	return effectField;
}


class AGSEffectField* UGSBlueprintFunctionLibrary::FinishCreateEffectField(class AGSEffectField* EffectField)
{
	if (EffectField)
	{
		FTransform Trans;
		EffectField->FinishSpawning(Trans);
		EffectField->InitializeEffectField();
	}
	return EffectField;
}