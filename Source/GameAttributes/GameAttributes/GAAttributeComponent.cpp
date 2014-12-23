// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameAttributes.h"
#include "GAAttributesBase.h"

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
}

void UGAAttributeComponent::ModifyAttributesOnSelf(const FGAAttributeModifier& AttributeIn)
{

	//apply final value to attribute.
	//after we appiled
	float newValue = DefaultAttributes->AttributeOperation(AttributeIn.Attribute, AttributeIn.Value, AttributeIn.Operation);
	DefaultAttributes->SetFloatValue(AttributeIn.Attribute, newValue);
}
void UGAAttributeComponent::ModifyAttributesOnTarget(UGAAttributeComponent* Target, const FGAAttributeModifier& AttributeIn)
{
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