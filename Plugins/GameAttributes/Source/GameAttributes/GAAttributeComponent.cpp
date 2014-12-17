// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GAAttributeComponent.h"

UGAAttributeComponent::UGAAttributeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bAutoRegister = true;
}

void UGAAttributeComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (DefaultAttributesClass)
	{
		DefaultAttributes = ConstructObject<UGAAttributesBase>(DefaultAttributesClass, this);
	}
}