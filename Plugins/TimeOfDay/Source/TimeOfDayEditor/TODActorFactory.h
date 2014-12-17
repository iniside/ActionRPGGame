// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once
//#include "Factories/Factory.h"
#include "UnrealEd.h"
#include "TODActorFactory.generated.h"

UCLASS()
class UTODActorFactory : public UActorFactory
{
	GENERATED_UCLASS_BODY()

		// Begin UActorFactory Interface
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
	virtual void PostCreateBlueprint(UObject* Asset, AActor* CDO) override;
	virtual UObject* GetAssetFromActorInstance(AActor* ActorInstance) override;
	//virtual FQuat AlignObjectToSurfaceNormal(const FVector& InSurfaceNormal, const FQuat& ActorRotation) const override;
	// End UActorFactory Interface
};
