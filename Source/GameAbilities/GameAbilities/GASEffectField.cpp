// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAbilities.h"

#include "GASAbility.h"

#include "GASEffectField.h"

AGASEffectField::AGASEffectField(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//replicate ?
	//or just spawn independently on each client ? and then trough delegate/RepNotify tell to destroy ?
	//bReplicates = true; 
	//SetReplicates(true);
}

void AGASEffectField::InitializeField()
{
	//1. Automaticallt gather all collision componenets from blueprinted version.
	// don't want to hardcode any kind of collision inside base class
	// let designer decide what exactly he wants to do with it.
	// but on other hand encapsulate this functionality so 
	// designer won't have to deal with binding/assiging events inside blueprint.
	//2. Bind Events from them.
}

void AGASEffectField::DestroyField()
{

}

void AGASEffectField::OnAbilityExecuted_Implementation()
{

}

void AGASEffectField::OnOtherFieldOverlap_Implementation()
{

}