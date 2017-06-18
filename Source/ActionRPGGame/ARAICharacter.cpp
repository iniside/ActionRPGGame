// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"

//#include "GAAbilitiesComponent.h"
#include "Items/ARItemInfo.h"
#include "Widgets/GISContainerBaseWidget.h"
#include "Items/GSEquipmentComponent.h"
#include "Attributes/GAAttributesBase.h"

#include "Items/GSEquipmentComponent.h"
#include "Weapons/GSWeaponEquipmentComponent.h"
#include "Components/GSActiveActionsComponent.h"

#include "Items/GSItemWeaponInfo.h"

#include "GSGlobalTypes.h"

//#include "Net/UnrealNetwork.h"
//#include "Engine/ActorChannel.h"
#include "ARCharacterMovementComponent.h"
#include "GSPlayerController.h"
#include "ARPlayerController.h"

#include "ARAICharacter.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

AARAICharacter::AARAICharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	// set our turn rates for input

	bReplicates = true;


	Attributes = ObjectInitializer.CreateDefaultSubobject<UGAAbilitiesComponent>(this, TEXT("Attributes"));
	Attributes->SetIsReplicated(true);
	Attributes->SetNetAddressable();
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

float AARAICharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	if (!EventInstigator && !DamageCauser)
	{
		ReceiveAnyDamage(DamageAmount, nullptr, EventInstigator, DamageCauser);
//		OnTakeAnyDamage.Broadcast(DamageAmount, nullptr, EventInstigator, DamageCauser);
		//it's coming from attribute component.
		//it means health reached zero (or whatever).
		Destroy(); //destroy this actor.
		//this will probabaly break game hehehehe.
	}
	return 0;
}

/** IIGAAbilities Begin */
class UGAAttributesBase* AARAICharacter::GetAttributes()
{
	return Attributes->DefaultAttributes;
}

class UGAAbilitiesComponent* AARAICharacter::GetAbilityComp()
{
	return Attributes;
}

float AARAICharacter::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return Attributes->DefaultAttributes->GetFloatValue(AttributeIn);
}
void AARAICharacter::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	, FGAEffectProperty& InProperty)
{
	Attributes->ModifyAttribute(ModIn, HandleIn, InProperty);
}
FAFAttributeBase* AARAICharacter::GetAttribute(FGAAttribute AttributeIn)
{
	return GetAttributes()->GetAttribute(AttributeIn);
}
void AARAICharacter::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod)
{
	GetAttributes()->RemoveBonus(AttributeIn, HandleIn, HandleIn.GetAttributeMod());
}
FGAEffectHandle AARAICharacter::ApplyEffectToTarget(FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{
	return GetAbilityComp()->ApplyEffectToTarget(EffectIn, InProperty, InContext);
};
void AARAICharacter::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	GetAbilityComp()->RemoveTagContainer(TagsIn);
}
float AARAICharacter::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return 0;
}