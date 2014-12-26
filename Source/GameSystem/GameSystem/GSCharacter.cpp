// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "GSCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

AGSCharacter::AGSCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}
