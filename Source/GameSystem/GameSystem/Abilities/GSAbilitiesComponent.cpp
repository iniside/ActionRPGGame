// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSAbility.h"
#include "../GSPlayerController.h"
#include "UserWidget.h"
#include "Net/UnrealNetwork.h"

#include "GSAbilitiesComponent.h"

UGSAbilitiesComponent::UGSAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}