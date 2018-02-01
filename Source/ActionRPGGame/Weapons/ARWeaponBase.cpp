// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponBase.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"
#include "AMAbilityManagerComponent.h"
#include "ARWeaponManagerComponent.h"
#include "../ARCharacter.h"
// Sets default values
AARWeaponBase::AARWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AARWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AARWeaponBase::Equip(EAMGroup Group, EAMSlot Slot
	, class APawn* InPOwner
	, class UAMAbilityManagerComponent* WeaponManager)
{
	UARWeaponManagerComponent* WManager = Cast<UARWeaponManagerComponent>(WeaponManager);
	if (!WManager)
		return;
	
	POwner = InPOwner;
	FSimpleDelegate Delegate = FSimpleDelegate::CreateUObject(this, &AARWeaponBase::NativeOnWeaponEquiped);
	WManager->AddOnAbilityReadyEvent(WeaponAbility, Delegate);
	WManager->NativeEquipAbility(WeaponAbility, Group, Slot, this);
}
void AARWeaponBase::NativeOnWeaponEquiped()
{
	if (!POwner)
		return;

	AARCharacter* Character = Cast<AARCharacter>(POwner);
	if (!Character)
		return;

	AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
	//Ability is ready, Attach to pawn.
}