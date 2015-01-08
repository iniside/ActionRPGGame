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
	if (DefaultAttributesClass)
	{
		DefaultAttributes = ConstructObject<UGAAttributesBase>(DefaultAttributesClass, this);
		//DefaultAttributes->SetNetAddressable();
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
	MulticastAttributeChanged();
}
void UGAAttributeComponent::ModifyAttributesOnTarget(UGAAttributeComponent* Target, FGAAttributeModifier& AttributeIn)
{
	//apply possible mods from
	OnAttributeOutgoing.Broadcast(AttributeIn, AttributeIn);
	Target->ModifyAttributesOnSelf(this, AttributeIn);

	//ModifiedAttribute.Attribute = AttributeIn.Attribute;
	//ModifiedAttribute.InstigatorLocation = AttributeIn.Instigator->GetActorLocation();
	//ModifiedAttribute.TargetLocation = AttributeIn.Target->GetActorLocation();
	//ModifiedAttribute.ModifiedByValue = AttributeIn.Value;
	//ModifiedAttribute.Tags = AttributeIn.Tags;
	//ModifiedAttribute.ReplicationCounter += 1;

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