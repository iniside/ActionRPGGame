// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "GACharacterAttributeTest.h"


// Sets default values
AGACharacterAttributeTest::AGACharacterAttributeTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UAFAbilityComponent>(TEXT("Attributes"));
}

// Called when the game starts or when spawned
void AGACharacterAttributeTest::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGACharacterAttributeTest::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

// Called to bind functionality to input
void AGACharacterAttributeTest::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

class UGAAttributesBase* AGACharacterAttributeTest::GetAttributes()
{
	return Attributes->DefaultAttributes;
}
class UAFAbilityComponent* AGACharacterAttributeTest::GetAbilityComp()
{
	return Attributes;
}
void AGACharacterAttributeTest::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
	FGAEffectProperty& InProperty)
{ 
	GetAttributes()->ModifyAttribute(ModIn, HandleIn, InProperty);
}

FAFAttributeBase* AGACharacterAttributeTest::GetAttribute(FGAAttribute AttributeIn)
{ 
	return GetAttributes()->GetAttribute(AttributeIn);
}
void AGACharacterAttributeTest::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod)
{
	GetAttribute(AttributeIn)->RemoveBonus(HandleIn, InMod);
}
FGAEffectHandle AGACharacterAttributeTest::ApplyEffectToTarget(FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{ 
	return GetAbilityComp()->ApplyEffectToTarget(EffectIn, InProperty, InContext);
};
void AGACharacterAttributeTest::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	GetAbilityComp()->RemoveTagContainer(TagsIn);
}
float AGACharacterAttributeTest::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{ 
	return 0;
}