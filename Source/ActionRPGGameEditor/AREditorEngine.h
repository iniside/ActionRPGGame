// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "UnrealEd.h"
#include "AREditorEngine.generated.h"

UCLASS()
class UAREditorEngine : public UEditorEngine
{
	GENERATED_UCLASS_BODY()

	virtual void Init(IEngineLoop* InEngineLoop) override;
};



