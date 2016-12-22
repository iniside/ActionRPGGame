// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "../GAAbilitiesComponent.h"
#include "GACharacterAttributeTest.h"


// Sets default values
AGACharacterAttributeTest::AGACharacterAttributeTest()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Attributes = CreateDefaultSubobject<UGAAbilitiesComponent>(TEXT("Attributes"));
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
void AGACharacterAttributeTest::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

}

class UGAAttributesBase* AGACharacterAttributeTest::GetAttributes()
{
	return Attributes->DefaultAttributes;
}
class UGAAbilitiesComponent* AGACharacterAttributeTest::GetAbilityComp()
{
	return Attributes;
}