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

	//if (AttributeMods.Num() > 0)
	//{
	//	for (TSubclassOf<UGAAttributeMod> mod : AttributeMods)
	//	{
	//		UGAAttributeMod* modObj = ConstructObject<UGAAttributeMod>(mod, this);
	//		modObj->BindDelegates(OnAttributeOutgoing, OnAttributeIncoming);
	//		AttributeModsObj.Add(modObj);
	//	}
	//}
}

void UGAAttributeComponent::ModifyAttributesOnSelf(UGAAttributeComponent* Causer, FGAAttributeModData& AttributeIn)
{

	//apply final value to attribute.
	//after we appiled
	ENetRole role = GetOwnerRole();
	ModifiedAttribute.Empty(); //don't need anything old laying around.
	for (FGAAttributeSpec& spec : AttributeIn.AttributeModSpec)
	{
		FGAAttributeSpec ModdedSpecOut;
		DefaultAttributes->CalculateIncomingAttributeMods(spec, ModdedSpecOut);

		float newValue = DefaultAttributes->AttributeOperation(ModdedSpecOut.Attribute, ModdedSpecOut.ModValue, ModdedSpecOut.Operation);
		
		FGAEvalData attrSet = FGAEvalData(AttributeIn.AttributeContext);
		attrSet.Attribute = ModdedSpecOut.Attribute;
		attrSet.ModValue = ModdedSpecOut.ModValue;
		DefaultAttributes->UpdateAttributes(attrSet, newValue);
		
		FGAModifiedAttribute ModdedAttrRep;
		ModdedAttrRep.Attribute = ModdedSpecOut.Attribute;
		ModdedAttrRep.InstigatorLocation = AttributeIn.AttributeContext.Instigator->GetOwner()->GetActorLocation();
		ModdedAttrRep.TargetLocation = AttributeIn.HitLocation;//AttributeIn.Target->GetActorLocation();
		ModdedAttrRep.ModifiedByValue = ModdedSpecOut.ModValue;
		ModdedAttrRep.Tags = AttributeIn.Tags;
		ModdedAttrRep.ReplicationCounter += 1;
		ModdedAttrRep.Causer = Causer;
		ModifiedAttribute.Add(ModdedAttrRep);

		FGAUpdatedAttribute attributeUpdate;
		attributeUpdate.Attribute = ModdedSpecOut.Attribute;
		attributeUpdate.NewValue = newValue;
		OnAttributeUpdated.Broadcast(attributeUpdate);
	}


	////check if if this actor is dead.
	//if (DefaultAttributes->GetFloatValue(DeathAttribute) <= 0)
	//{
	//	if (AttributeInterface)
	//	{
	//		AttributeInterface->Died();
	//	}
	//}
	if (GetNetMode() == ENetMode::NM_Standalone)
	{
		for (FGAModifiedAttribute& attr : ModifiedAttribute)
		{
			Causer->OnAttributeModifed.Broadcast(attr);
		}
	}
}
void UGAAttributeComponent::ModifyAttributesOnTarget(UGAAttributeComponent* Target, FGAAttributeModData& AttributeIn)
{
	if (!DefaultAttributes)
		return;
	TArray<FGAAttributeSpec> ModdedSpecs;
	for (FGAAttributeSpec& spec : AttributeIn.AttributeModSpec)
	{
		FGAAttributeSpec ModdedSpecOut;
		DefaultAttributes->CalculateOutgoingAttributeMods(spec, ModdedSpecOut);

		ModdedSpecs.Add(ModdedSpecOut);
	}
	AttributeIn.AttributeModSpec = ModdedSpecs;

	Target->ModifyAttributesOnSelf(this, AttributeIn);
}


void UGAAttributeComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UGAAttributeComponent, DefaultAttributes);
	DOREPLIFETIME(UGAAttributeComponent, ModifiedAttribute);
}

void UGAAttributeComponent::OnRep_AttributeChanged()
{
	for (FGAModifiedAttribute& attr : ModifiedAttribute)
	{
		attr.Causer->OnAttributeModifed.Broadcast(attr);
	}
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
