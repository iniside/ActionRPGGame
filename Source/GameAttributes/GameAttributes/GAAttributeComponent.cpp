// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

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

	if (DefaultAttributesClass)
	{
		DefaultAttributes = ConstructObject<UGAAttributesBase>(DefaultAttributesClass, this);
		DefaultAttributes->SetNetAddressable();
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

void UGAAttributeComponent::ModifyAttributesOnSelf(FGAAttributeModifier& AttributeIn)
{

	//apply final value to attribute.
	//after we appiled
	OnAttributeIncoming.Broadcast(AttributeIn, AttributeIn);
	float newValue = DefaultAttributes->AttributeOperation(AttributeIn.Attribute, AttributeIn.Value, AttributeIn.Operation);
	DefaultAttributes->SetFloatValue(AttributeIn.Attribute, newValue);
}
void UGAAttributeComponent::ModifyAttributesOnTarget(UGAAttributeComponent* Target, FGAAttributeModifier& AttributeIn)
{
	//apply possible mods from
	OnAttributeOutgoing.Broadcast(AttributeIn, AttributeIn);
	Target->ModifyAttributesOnSelf(AttributeIn);
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