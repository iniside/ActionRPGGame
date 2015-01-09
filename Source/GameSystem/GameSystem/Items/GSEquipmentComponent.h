// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISInventoryBaseComponent.h"
#include "GSEquipmentComponent.generated.h"


UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSEquipmentComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Equips item. Which means it will be attached/replcaed on owner of this component.

		Probabaly need some fancy interface actually..
	*/
	void EquiptItem(USkeletalMesh* MeshIn);
};



