// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

//#include "GISGlobalTypes.h"
//#include "IGISPickupItem.h"
#include "GISPickupActor.h"
#include "ARItemPickup.generated.h"


USTRUCT(BlueprintType)
struct FARItemPickupData
{
	GENERATED_USTRUCT_BODY()
public:
//	UPROPERTY(EditAnywhere)
//		TSubclassOf<class UGISItemData> ContainerType;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UObject> > ItemsToPick;
};

UCLASS(Blueprintable, BlueprintType)
class AARItemPickup : public AGISPickupActor//, public IIGISPickupItem
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginPlay() override;
//public:
//	UPROPERTY(EditAnywhere)
//		TArray<FARItemPickupData> PickupItemData;

	//UPROPERTY(EditAnywhere, Instanced, meta = (ExposedOnSpawn))
	//	class UGISItemData* DataTest;
};
