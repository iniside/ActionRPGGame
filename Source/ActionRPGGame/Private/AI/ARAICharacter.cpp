// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAICharacter.h"
#include "AREnemySpawner.h"

// Sets default values
AARAICharacter::AARAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Abilities = CreateDefaultSubobject<UAFAbilityComponent>(TEXT("Abilities"));
	EffectsComponent = CreateDefaultSubobject<UAFEffectsComponent>(TEXT("EffectsComponent"));
}

// Called when the game starts or when spawned
void AARAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}
void AARAICharacter::OnSpawned(class AAREnemySpawner* InSpawnedBy)
{
	SpawnedBy = InSpawnedBy;
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

void AARAICharacter::Kill()
{
	if(SpawnedBy)
		SpawnedBy->OnEnemyKilled(this);

	Destroy();
}

/* IAFAbilityInterface- BEGIN */
class UAFAbilityComponent* AARAICharacter::GetAbilityComp()
{
	return Abilities;
};
class UAFEffectsComponent* AARAICharacter::GetEffectsComponent()
{
	return EffectsComponent;
}
class UAFEffectsComponent* AARAICharacter::NativeGetEffectsComponent() const
{
	return EffectsComponent;
}
float AARAICharacter::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return Abilities->GetAttributeValue(AttributeIn);
}

void AARAICharacter::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
	struct FGAEffectProperty& InProperty, const FGAEffectContext& InContex)
{
	Abilities->ModifyAttribute(ModIn, HandleIn, InProperty, InContex);
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
/* IAFAbilityInterface- END */