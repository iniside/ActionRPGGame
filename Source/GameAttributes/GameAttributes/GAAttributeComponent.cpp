// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

#include "IGAAttributes.h"

#include "Mods/GAAttributeMod.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "GAAttributeComponent.h"

//will have single struct param, with data about how attribute changed.


UGAAttributeComponent::UGAAttributeComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bAutoRegister = true;
}

void UGAAttributeComponent::InitializeComponent()
{
	Super::InitializeComponent();
	AttributeInterface = Cast<IIGAAttributes>(GetOwner());
	//if (DefaultAttributesClass)
	//{
	//	DefaultAttributes = ConstructObject<UGAAttributesBase>(DefaultAttributesClass, this);
	//	//DefaultAttributes->SetNetAddressable();
	//}

	if (DefaultAttributes)
	{
		DefaultAttributes->InitializeAttributes();
	}

	if (AttributeMods.Num() > 0)
	{
		for (TSubclassOf<UGAAttributeMod> mod : AttributeMods)
		{
			UGAAttributeMod* modObj = ConstructObject<UGAAttributeMod>(mod, this);
			modObj->BindDelegates(OnAttributeOutgoing, OnAttributeIncoming);
			AttributeModsObj.Add(modObj);
		}
	}
}

void UGAAttributeComponent::ModifyAttributesOnSelf(UGAAttributeComponent* Causer, FGAAttributeModifier& AttributeIn)
{

	//apply final value to attribute.
	//after we appiled
	ENetRole role = GetOwnerRole();

	OnAttributeIncoming.Broadcast(AttributeIn, AttributeIn);
	float newValue = DefaultAttributes->AttributeOperation(AttributeIn.Attribute, AttributeIn.Value, AttributeIn.Operation);
	DefaultAttributes->SetFloatValue(AttributeIn.Attribute, newValue);

	//check if if this actor is dead.
	if (DefaultAttributes->GetFloatValue(DeathAttribute) <= 0)
	{
		if (AttributeInterface)
		{
			AttributeInterface->Died();
		}
	}

	ModifiedAttribute.Attribute = AttributeIn.Attribute;
	ModifiedAttribute.InstigatorLocation = AttributeIn.Instigator->GetActorLocation();
	ModifiedAttribute.TargetLocation = AttributeIn.HitTarget.Location; //AttributeIn.Target->GetActorLocation();
	ModifiedAttribute.ModifiedByValue = AttributeIn.Value;
	ModifiedAttribute.Tags = AttributeIn.Tags;
	ModifiedAttribute.ReplicationCounter += 1;
	ModifiedAttribute.Causer = Causer;
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		Causer->OnAttributeModifed.Broadcast(ModifiedAttribute);
	}
	FGAUpdatedAttribute attributeUpdate;
	attributeUpdate.Attribute = AttributeIn.Attribute;
	attributeUpdate.NewValue = newValue;
	OnAttributeUpdated.Broadcast(attributeUpdate);

	MulticastAttributeChanged();
}
void UGAAttributeComponent::ModifyAttributesOnTarget(UGAAttributeComponent* Target, FGAAttributeModifier& AttributeIn)
{
	//apply possible mods from
	//OnAttributeOutgoing.Broadcast(AttributeIn, AttributeIn); //In, Out. to Complicated.
	Target->ModifyAttributesOnSelf(this, AttributeIn);

	/*
		So they idea is this.
		We iterate over all mods, executing mod function.
		Incoming attribute is moded, returned, and then moded by next object,
		in order those objects were added.

		Therepractically two ways of doing it.

		Create one uber function, in C++, which will, do everything,
		or create lots of small objects.

		Uber function is probabaly better, once you figured stuff out.

		In anycase there two sets of functions, for modiging outgoing changes (from Source).
		and for modiging incoming changes (On Target).

		Some things, we don't want to be appilied from Source (for example, target damage reduction by source
		armor)
	*/
	for (UGAAttributeMod* mod : AttributeMods)
	{
		mod->OnAttributeModifyOut(AttributeIn, AttributeIn); //In, out, 
		//probabaly should just do return value..
	}

	//if (GetNetMode() == ENetMode::NM_Standalone)
	//{
	//	OnAttributeModifed.Broadcast(ModifiedAttribute);
	//}
}


void UGAAttributeComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UGAAttributeComponent, DefaultAttributes);
	DOREPLIFETIME(UGAAttributeComponent, ModifiedAttribute);
}
void UGAAttributeComponent::MulticastAttributeChanged_Implementation()
{
//	if (ModifiedAttribute.Causer->OnAttributeModifed.IsBound())
//	{
	//	ModifiedAttribute.Causer->OnAttributeModifed.Broadcast(ModifiedAttribute);
	//}
	//OnAttributeModifed.Broadcast(ModifiedAttribute);
}
void UGAAttributeComponent::OnRep_AttributeChanged()
{
	ModifiedAttribute.Causer->OnAttributeModifed.Broadcast(ModifiedAttribute);
}
bool UGAAttributeComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (DefaultAttributes)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(DefaultAttributes), *Bunch, *RepFlags);
	}
	return WroteSomething;
}
void UGAAttributeComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (DefaultAttributes && DefaultAttributes->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UGAAttributesBase*>(DefaultAttributes));
	}
}
