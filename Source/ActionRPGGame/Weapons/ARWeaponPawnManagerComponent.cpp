// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponPawnManagerComponent.h"

#include "../ARCharacter.h"

// Sets default values for this component's properties
UARWeaponPawnManagerComponent::UARWeaponPawnManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	WeaponHelper.Add(&Group001HolsteredAttachment);
	WeaponHelper.Add(&Group002HolsteredAttachment);
	WeaponHelper.Add(&Group003HolsteredAttachment);
	WeaponHelper.Add(&Group004HolsteredAttachment);
	// ...
}


// Called when the game starts
void UARWeaponPawnManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UARWeaponPawnManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARWeaponPawnManagerComponent::SetWeapon(TSoftObjectPtr<USkeletalMesh> InWeapon, USkeletalMeshComponent* Component)
{
	if (POwner)
	{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			Component->SetSkeletalMesh(InWeapon.Get());
		}
	}
}

void UARWeaponPawnManagerComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UARWeaponPawnManagerComponent, Group001HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponPawnManagerComponent, Group002HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponPawnManagerComponent, Group003HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponPawnManagerComponent, Group004HolsteredAttachment, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(UARWeaponPawnManagerComponent, MainHandWeapon, COND_SkipOwner);
}

void UARWeaponPawnManagerComponent::Equip(EAMGroup Group, TSoftObjectPtr<USkeletalMesh> InWeapon)
{
	MainHandWeapon.WeaponMesh = InWeapon;
	MainHandWeapon.RepCounter++;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(InWeapon, Character->GetEquipedMainWeapon());
	}
}

void UARWeaponPawnManagerComponent::Holster(EAMGroup Group, TSoftObjectPtr<USkeletalMesh> InWeapon)
{
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->WeaponMesh = InWeapon;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(InWeapon, Character->GetHolsteredRightWeapon());
	}
}

void UARWeaponPawnManagerComponent::OnRep_Group001HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group001HolsteredAttachment.WeaponMesh, Character->GetHolsteredRightWeapon());
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group002HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group002HolsteredAttachment.WeaponMesh, Character->GetHolsteredRightWeapon());
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group003HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group003HolsteredAttachment.WeaponMesh, Character->GetHolsteredRightWeapon());
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group004HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group004HolsteredAttachment.WeaponMesh, Character->GetHolsteredRightWeapon());
	}
}

void UARWeaponPawnManagerComponent::OnRep_MainHandWeapon()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(MainHandWeapon.WeaponMesh, Character->GetHolsteredRightWeapon());
	}
}