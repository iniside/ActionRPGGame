// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "GSCharacter.generated.h"

UCLASS(config=Game)
class AGSCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	AGSCharacter(const FObjectInitializer& ObjectInitializer);
};

