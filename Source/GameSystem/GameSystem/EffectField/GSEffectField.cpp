// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "IGAAttributes.h"
#include "IGSEffectField.h"

#include "GAAttributeComponent.h"

#include "GSEffectField.h"

AGSEffectField::AGSEffectField(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	bReplicates = true;
	OverlapingActorCount = 0;

	//since there is possibility that there are quite a few of these actors at once in world
	//I probabaly should tick them by default..

	Attributes = ObjectInitializer.CreateDefaultSubobject<UGAAttributeComponent>(this, TEXT("Attributes"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;
}

void AGSEffectField::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CurrentLifetime += DeltaSeconds;
	//if lifetime is less than zero, it means that this actor should
	//live until manually removed from world.
	if ((LifeTime > 0) && (CurrentLifetime > LifeTime))
	{
		DestroyField();
	}
	for (auto It = OverlapingActors.CreateIterator(); It; ++It)
	{
		OnActorHit(OverlapingActors[It.GetIndex()]);
	}
}

void AGSEffectField::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	if (!RootComponent)
		return;

	if (!FieldSize.IsZero())
	{
		UBoxComponent* BoxComp = Cast<UBoxComponent>(RootComponent);
		if (BoxComp)
		{
			//BoxComp->InitBoxExtent(FieldSize);
			BoxComp->SetBoxExtent(FieldSize);

		}
		else if (USphereComponent* SphereComp = Cast<USphereComponent>(RootComponent))
		{
			if (FieldSize.X > 0)
			{
				SphereComp->SetSphereRadius(FieldSize.X);
			}
			else if (FieldSize.Y > 0)
			{
				SphereComp->SetSphereRadius(FieldSize.X);
			}
			else if (FieldSize.Z > 0)
			{
				SphereComp->SetSphereRadius(FieldSize.X);
			}
		}
		else if (UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(RootComponent))
		{
			CapsuleComp->SetCapsuleSize(FieldSize.X, FieldSize.Y);
		}
	}
	FVector ActorLocation = GetActorLocation();
	ActorLocation.Z += FieldSize.Z;
	RootComponent->SetRelativeLocation(ActorLocation);
}

void AGSEffectField::BeginDestroy()
{
	FieldInt = nullptr;
	Super::BeginDestroy();
}

void AGSEffectField::InitializeEffectField()
{		
	OverlapingActors.Empty();
	UPrimitiveComponent* primComp = Cast<UPrimitiveComponent>(RootComponent);
	if (!primComp)
		return;
	primComp->OnComponentHit.AddDynamic(this, &AGSEffectField::OnFieldHit);
	primComp->OnComponentBeginOverlap.AddDynamic(this, &AGSEffectField::OnFieldBeginOverlap);
	primComp->OnComponentEndOverlap.AddDynamic(this, &AGSEffectField::OnFieldEndOverlap);

	if (!EffectFieldInstigator)
		return;

	FieldInt = Cast<IIGSEffectField>(EffectFieldInstigator);

	if (LifeTime > 0)
	{
		SetActorTickEnabled(true); //effect life is above 0 so we probabaly want to tick it anyway.

		//permament fields probabaly don't need to tick, if they do, enable it explictly.
	}
}

void AGSEffectField::DestroyField()
{

}

void AGSEffectField::BP_DestroyField()
{
	DestroyField();
}

void AGSEffectField::OnFieldHit(class AActor* OtherActor, class UPrimitiveComponent* HitComponent, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	float test = 1;
}

void AGSEffectField::OnFieldBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/*
		Check if other actor is field.
	*/
	if (AGSEffectField* otherField = Cast<AGSEffectField>(OtherActor))
	{
		OnOtherFieldOverlap(otherField);
		return; //if it fire event, and return. Nothing to do here.
	}
	/*
		If actor doesn't have tags, then we probabaly can't affect it ?

		Maybe better check it against attributes ? Like if it have attribute component
		then we probabaly can do something about it.
	*/
	IIGAAttributes* attrInt = Cast<IIGAAttributes>(OtherActor);
	if (attrInt)
	{
		OverlapingActorCount += OverlapingActors.AddUnique(OtherActor); //pretty much always should be 1
		//OverlapingActors.Add(OtherActor);
	}
	float test = 1;
}


void AGSEffectField::OnFieldEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	OverlapingActorCount -= OverlapingActors.Remove(OtherActor);//pretty much always should be 1
	float test = 1;
}

void AGSEffectField::OnOtherFieldOverlap(AGSEffectField* OtherField)
{
	BP_OnOtherFieldOverlap(OtherField);
}

class UGAAttributesBase* AGSEffectField::GetAttributes()
{
	return Attributes->DefaultAttributes;
}

class UGAAttributeComponent* AGSEffectField::GetAttributeComponent()
{
	return Attributes;
}

void AGSEffectField::Died()
{
	DestroyField();
}

void AGSEffectField::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = MyTags;
}

bool AGSEffectField::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return MyTags.HasTag(TagToCheck, EGameplayTagMatchType::Explicit, EGameplayTagMatchType::Explicit);
}
bool AGSEffectField::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch) const
{
	return MyTags.MatchesAll(TagContainer, bCountEmptyAsMatch);
}

bool AGSEffectField::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch) const
{
	return MyTags.MatchesAny(TagContainer, bCountEmptyAsMatch);
}