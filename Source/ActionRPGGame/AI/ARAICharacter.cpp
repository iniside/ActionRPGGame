// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAICharacter.h"


// Sets default values
AARAICharacter::AARAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Abilities = CreateDefaultSubobject<UAFAbilityComponent>(TEXT("Abilities"));
}

// Called when the game starts or when spawned
void AARAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AARAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

/* IAFAbilityInterface- BEGIN */

class UGAAttributesBase* AARAICharacter::GetAttributes()
{
	return GetAbilityComp()->DefaultAttributes;
}

class UAFAbilityComponent* AARAICharacter::GetAbilityComp()
{
	return Abilities;
};

float AARAICharacter::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return Abilities->GetAttributeValue(AttributeIn);
}

void AARAICharacter::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
	struct FGAEffectProperty& InProperty)
{
	Abilities->ModifyAttribute(ModIn, HandleIn, InProperty);
}

FAFAttributeBase* AARAICharacter::GetAttribute(FGAAttribute AttributeIn)
{
	return Abilities->GetAttribute(AttributeIn);
};

void AARAICharacter::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod)
{
	Abilities->RemoveBonus(AttributeIn, HandleIn, InMod);
}

float AARAICharacter::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return Abilities->NativeGetAttributeValue(AttributeIn);
}

FGAEffectHandle AARAICharacter::ApplyEffectToTarget(FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{
	return Abilities->ApplyEffectToTarget(EffectIn, InProperty, InContext);
}

void AARAICharacter::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	Abilities->RemoveTagContainer(TagsIn);
}
/* IAFAbilityInterface- END */