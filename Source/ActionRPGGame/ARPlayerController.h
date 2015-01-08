// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "FCTFloatingTextComponent.h"
#include "GSPlayerController.h"
#include "FCTGlobalTypes.h"
#include "GAGlobalTypes.h"
#include "ARPlayerController.generated.h"


UCLASS()
class ACTIONRPGGAME_API AARPlayerController : public AGSPlayerController
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginPlay() override;
	virtual void OnRep_Pawn() override;

	UFUNCTION()
		void OnRecivedModifiedAttribute(const FGAModifiedAttribute& AttributeModIn);

	//virtual void PreInitializeComponents() override;
};



