// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "IFItemBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYFRAMEWORK_API UIFItemBase : public UObject
{
	GENERATED_BODY()
public:
	bool IsNameStableForNetworking() const override
	{
		return false;
	}

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	
	
};
