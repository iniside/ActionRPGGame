// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "UnrealEd.h"
#include "ARUnrealEdEngine.generated.h"

UCLASS()
class ACTIONRPGGAMEEDITOR_API UARUnrealEdEngine : public UUnrealEdEngine
{
	GENERATED_UCLASS_BODY()

	virtual void Init(IEngineLoop* InEngineLoop) override;
};



