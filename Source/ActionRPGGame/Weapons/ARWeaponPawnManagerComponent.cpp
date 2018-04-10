// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponPawnManagerComponent.h"
#include "Engine/AssetManager.h"
#include "ARItemWeapon.h"
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
	if (AARCharacter* Character = Cast<AARCharacter>(GetOwner()))
	{
		GroupToComponent.Add(EAMGroup::Group001, Character->GetHolsteredRightWeapon());
		GroupToComponent.Add(EAMGroup::Group002, Character->GetHolsteredLeftWeapon());
		GroupToComponent.Add(EAMGroup::Group003, Character->GetHolsteredBackDownWeapon());
		GroupToComponent.Add(EAMGroup::Group004, Character->GetHolsteredSideLeftWeapon());

		GroupToItem.Add(EAMGroup::Group001, Character->WeaponRightItem);
		GroupToItem.Add(EAMGroup::Group002, Character->WeaponLeftItem);
		GroupToItem.Add(EAMGroup::Group003, Character->WeaponBackItem);
		GroupToItem.Add(EAMGroup::Group004, Character->WeaponSideItem);
	}
}


// Called every frame
void UARWeaponPawnManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// ...
}

void UARWeaponPawnManagerComponent::SetWeapon(const FARWeapon& InWeapon, UChildActorComponent* Component)
{
	if (InWeapon.Weapon.IsValid() || InWeapon.Weapon.IsNull())
	{
		Component->SetChildActorClass(InWeapon.Weapon.Get());
		Component->SetRelativeLocation(FVector(0, 0, 0));
		Component->SetRelativeRotation(FRotator(0, 0, 0));

		Component->SetRelativeLocation(InWeapon.Position);
		Component->SetRelativeRotation(InWeapon.Rotation);
	}
	else
	{
		FStreamableDelegate LoadFinished = FStreamableDelegate::CreateUObject(this, &UARWeaponPawnManagerComponent::AsynWeaponLoaded, Component, InWeapon);
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(InWeapon.Weapon.ToSoftObjectPath(), LoadFinished);
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

void UARWeaponPawnManagerComponent::Equip(EAMGroup Group, class UARItemWeapon* InWeapon)
{
	MainHandWeapon.Weapon = InWeapon->Weapon;
	MainHandWeapon.Position = InWeapon->EquipedPosition;
	MainHandWeapon.Rotation = InWeapon->EquipedRotation;
	MainHandWeapon.Group = Group;
	MainHandWeapon.RepCounter++;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Reset();
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
		GroupToComponent[Group]->SetChildActorClass(nullptr);
	}
}
void UARWeaponPawnManagerComponent::EquipInactive(EAMGroup Group, UARItemWeapon* InWeapon, EAMGroup OldGroup, UARItemWeapon* InOldWeapon)
{
	if (InOldWeapon)
	{
		WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Weapon = MainHandWeapon.Weapon;
		//WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Position = InOldWeapon->HolsteredPosition;
		//WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Rotation = InOldWeapon->HolsteredRotation;
		WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->RepCounter++;
		//GroupToComponent[OldGroup]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)]->Weapon.Get());
		SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(OldGroup)], GroupToComponent[OldGroup]);
	}

	MainHandWeapon.Weapon = WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon;
	MainHandWeapon.Position = InWeapon->EquipedPosition;
	MainHandWeapon.Rotation = InWeapon->EquipedRotation;
	MainHandWeapon.Group = Group;
	MainHandWeapon.RepCounter++;

	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Reset();
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
	//GroupToComponent[Group]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Get());
	SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(Group)], GroupToComponent[Group]);

	if(!InOldWeapon)
	{
		WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Reset();
		WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
		//GroupToComponent[Group]->SetChildActorClass(WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon.Get());
		SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(Group)], GroupToComponent[Group]);
	}
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		
		SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
		//GroupToComponent[Group]->SetChildActorClass(nullptr);
	}
}
void UARWeaponPawnManagerComponent::Holster(EAMGroup Group, class UARItemWeapon* InWeapon)
{
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Weapon = InWeapon->Weapon;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Item = InWeapon->GetClass();
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Position = InWeapon->HolsteredPosition;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Rotation = InWeapon->HolsteredRotation;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->Group = Group;
	WeaponHelper[AMEnumToInt<EAMGroup>(Group)]->RepCounter++;
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		//Character->GetHolsteredRightWeapon()->SetSkeletalMesh(nullptr);
		SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(Group)], GroupToComponent[Group]);
		if (Group == EAMGroup::Group001)
		{
			Character->WeaponRightItem = DuplicateObject<UARItemWeapon>(InWeapon, Character);
		}
	}
}
void UARWeaponPawnManagerComponent::HolsterActive(EAMGroup Group)
{
	WeaponHelper[AMEnumToInt<EAMGroup>(MainHandWeapon.Group)]->Weapon = MainHandWeapon.Weapon;
	WeaponHelper[AMEnumToInt<EAMGroup>(MainHandWeapon.Group)]->Group = MainHandWeapon.Group;
	WeaponHelper[AMEnumToInt<EAMGroup>(MainHandWeapon.Group)]->RepCounter++;

	MainHandWeapon.Weapon.Reset();
	MainHandWeapon.RepCounter++;

	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		Character->GetEquipedMainWeapon()->SetChildActorClass(nullptr);
		SetWeapon(*WeaponHelper[AMEnumToInt<EAMGroup>(MainHandWeapon.Group)], GroupToComponent[MainHandWeapon.Group]);
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group001HolsteredAttachment()
{
	///if (Group001HolsteredAttachment.WeaponMesh.IsPending())
	///{
		if (AARCharacter* Character = Cast<AARCharacter>(POwner))
		{
			SetWeapon(Group001HolsteredAttachment, Character->GetHolsteredRightWeapon());
		}
	///}
}
void UARWeaponPawnManagerComponent::OnRep_Group002HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group002HolsteredAttachment, Character->GetHolsteredLeftWeapon());
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group003HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group003HolsteredAttachment, Character->GetHolsteredRightWeapon());
	}
}
void UARWeaponPawnManagerComponent::OnRep_Group004HolsteredAttachment()
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		SetWeapon(Group004HolsteredAttachment, Character->GetHolsteredRightWeapon());
	}
}

void UARWeaponPawnManagerComponent::OnRep_MainHandWeapon(FARWeapon OldWeapon)
{
	if (AARCharacter* Character = Cast<AARCharacter>(POwner))
	{
		//SetWeapon(OldWeapon, GroupToComponent[OldWeapon.Group]);
		SetWeapon(MainHandWeapon, Character->GetEquipedMainWeapon());
		//GroupToComponent[MainHandWeapon.Group]->SetChildActorClass(nullptr);
	}
}

void UARWeaponPawnManagerComponent::AsynWeaponLoaded(UChildActorComponent* Component, FARWeapon InWeapon)
{
	Component->SetChildActorClass(InWeapon.Weapon.Get());
	
	Component->SetRelativeLocation(FVector(0,0,0));
	Component->SetRelativeRotation(FRotator(0,0,0));

	Component->SetRelativeLocation(InWeapon.Position);
	Component->SetRelativeRotation(InWeapon.Rotation);
}