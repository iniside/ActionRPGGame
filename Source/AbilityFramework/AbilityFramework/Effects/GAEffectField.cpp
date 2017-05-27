// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"

#include "../Abilities/GAAbilityBase.h"

#include "GAEffectField.h"

AGAEffectField::AGAEffectField(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true; 
	SetReplicates(true);
}

void AGAEffectField::InitializeField()
{
	//if (bIsFieldPersistent && AbilityInstigator)
	//{

	//}
	//else if (!bIsFieldPersistent && AbilityInstigator)
	//{
	//	AbilityInstigator->OnAbilityCastEnd.AddUObject(this, &AGAEffectField::DestroyField);
	//}

	//1. Automaticallt gather all collision componenets from blueprinted version.
	// don't want to hardcode any kind of collision inside base class
	// let designer decide what exactly he wants to do with it.
	// but on other hand encapsulate this functionality so 
	// designer won't have to deal with binding/assiging events inside blueprint.
	//2. Bind Events from them.
}

void AGAEffectField::DestroyField()
{

}

void AGAEffectField::OnAbilityExecuted_Implementation()
{

}

void AGAEffectField::OnOtherFieldOverlap_Implementation()
{

}