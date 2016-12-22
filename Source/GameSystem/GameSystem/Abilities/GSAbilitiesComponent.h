// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GAAbilitiesComponent.h"
#include "GSAbilityGlobalTypes.h"
#include "GSAbilitiesComponent.generated.h"

DECLARE_DELEGATE(FGSOnAbilityAddedToSet);
DECLARE_DELEGATE_OneParam(FGSOnGetAbilityIndex, int32);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGSOnAbilityPressedIndex, int32, Index);
/* Blueprintable.. becaue why not ? Not that there are many extension points right now ;) */
UCLASS(Blueprintable, hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSAbilitiesComponent : public UGAAbilitiesComponent
{
	GENERATED_BODY()
public:
	UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer);
};



