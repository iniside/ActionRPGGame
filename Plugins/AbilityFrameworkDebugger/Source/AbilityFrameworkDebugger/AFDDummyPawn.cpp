// Fill out your copyright notice in the Description page of Project Settings.

#include "AFDDummyPawn.h"
#include "Attributes/GAAttributesBase.h"

// Sets default values
AAFDDummyPawn::AAFDDummyPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Arrow = ObjectInitializer.CreateDefaultSubobject<UArrowComponent>(this, TEXT("Arrow"));
	Collision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, TEXT("Collision"));
	Abilities = ObjectInitializer.CreateDefaultSubobject<UAFAbilityComponent>(this, TEXT("Abilities"));

	SetRootComponent(Arrow);
	Collision->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAFDDummyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAFDDummyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAFDDummyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
/* AFAbilityInterface Implementation START */
class UGAAttributesBase* AAFDDummyPawn::GetAttributes()
{
	return Abilities->GetAttributes();
}

class UAFAbilityComponent* AAFDDummyPawn::GetAbilityComp() 
{ 
	return Abilities; 
}

float AAFDDummyPawn::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return Abilities->GetAttributeValue(AttributeIn);
}
void AAFDDummyPawn::ModifyAttribute(FGAEffectMod& ModIn
	, const FGAEffectHandle& HandleIn
	, struct FGAEffectProperty& InProperty)
{
	GetAttributes()->ModifyAttribute(ModIn, HandleIn, InProperty);
}

FAFAttributeBase* AAFDDummyPawn::GetAttribute(FGAAttribute AttributeIn) 
{ 
	return Abilities->GetAttribute(AttributeIn);
};

void AAFDDummyPawn::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) 
{
	Abilities->RemoveBonus(AttributeIn, HandleIn, InMod);
};

float AAFDDummyPawn::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{ 
	return Abilities->NativeGetAttributeValue(AttributeIn); 
};

FGAEffectHandle AAFDDummyPawn::ApplyEffectToTarget(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty
	, FGAEffectContext& InContext)
{
	return Abilities->ApplyEffectToTarget(EffectIn, InProperty, InContext);
};
/* AFAbilityInterface Implementation END */