// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "UObject/NoExportTypes.h"
#include "GameObjectBase.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, config = Engine, meta = (ShortTooltip = "Simple object which can contain components."))
class ACTIONRPGGAME_API UGameObjectBase : public UObject
{
	GENERATED_BODY()
};
