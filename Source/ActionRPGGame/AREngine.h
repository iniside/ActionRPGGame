// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "AREngine.generated.h"

UCLASS()
class UAREngine : public UGameEngine
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Init(IEngineLoop* InEngineLoop) override;
};



