// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"
#include "ARCharacter.h"

#include "Abilities/ARAbilityManagerComponent.h"

#include "UI/ARHUD.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	AbilityManager = ObjectInitializer.CreateDefaultSubobject<UARAbilityManagerComponent>(this, "AbilityManager");
	MainInventory = ObjectInitializer.CreateDefaultSubobject<UIFInventoryComponent>(this, "MainInventory");
	MainInventory->SetIsReplicated(true);

	AbilityManager->ComponentTags.Add(TEXT("AbilityManager"));
	bInputBount = false;
}
void AARPlayerController::BeginPlay()
{
	Super::BeginPlay();
	ENetMode NetMode = GetNetMode();
	MainInventory->SetIsReplicated(true);
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
		if (!ABInt)
			return;

		UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
		if (!AbilityComp)
			return;

		if (!bInputBount)
		{
			AbilityComp->BindAbilityToAction(InputComponent, InputNextWeapon);
			AbilityComp->BindAbilityToAction(InputComponent, InputPreviousWeapon);
			AbilityComp->BindAbilityToAction(InputComponent, InputHolsterWeapon);
			AbilityComp->BindAbilityToAction(InputComponent, InputSetAbilityGroup01);
			AbilityComp->BindAbilityToAction(InputComponent, InputSetAbilityGroup02);


			AbilityManager->BindInputs(InputComponent, AbilityComp);
			AARCharacter* Character = Cast<AARCharacter>(GetPawn());

			Character->WeaponInventory->BindInputs(InputComponent, AbilityComp);

			bInputBount = true;
		}
		//doesn't matter. Internally ability component make sure abilities are instanced on server and replicated back.
		if (!AbilitytNextWeapon.IsNull())
		{
			FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytNextWeapon, InputNextWeapon);
			AbilityComp->AddOnAbilityReadyDelegate(AbilitytNextWeapon, del1);
			TArray<FGameplayTag> NextWeap;
			NextWeap.Add(InputNextWeapon);
			AbilityComp->NativeAddAbility(AbilitytNextWeapon, NextWeap);
		}
		if (!AbilitytPreviousWeapon.IsNull())
		{
			FAFOnAbilityReady del2 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytPreviousWeapon, InputPreviousWeapon);
			AbilityComp->AddOnAbilityReadyDelegate(AbilitytPreviousWeapon, del2);
			TArray<FGameplayTag> PrevWeap;
			PrevWeap.Add(InputPreviousWeapon);
			AbilityComp->NativeAddAbility(AbilitytPreviousWeapon, PrevWeap);
		}
		if (!AbilitytHolstersWeapon.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytHolstersWeapon, InputHolsterWeapon);
			AbilityComp->AddOnAbilityReadyDelegate(AbilitytHolstersWeapon, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputHolsterWeapon);
			AbilityComp->NativeAddAbility(AbilitytHolstersWeapon, HolsterInput);
		}

		if (!SetAbilityGroup01.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, SetAbilityGroup01, InputSetAbilityGroup01);
			AbilityComp->AddOnAbilityReadyDelegate(SetAbilityGroup01, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputSetAbilityGroup01);
			AbilityComp->NativeAddAbility(SetAbilityGroup01, HolsterInput);
		}
		if (!SetAbilityGroup02.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, SetAbilityGroup02, InputSetAbilityGroup02);
			AbilityComp->AddOnAbilityReadyDelegate(SetAbilityGroup02, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputSetAbilityGroup02);
			AbilityComp->NativeAddAbility(SetAbilityGroup02, HolsterInput);
		}

	}
}
void AARPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	
	//UIAbilityManagerComponent->BindInputs();
}
void AARPlayerController::Possess(APawn* aPawn)
{
	Super::Possess(aPawn);
	ENetMode NetMode = GetNetMode();
	

	if (NetMode == ENetMode::NM_DedicatedServer
		|| NetMode == ENetMode::NM_ListenServer)
	{
		ClientPossesed(aPawn);
	}
}

void AARPlayerController::ClientPossesed_Implementation(APawn* InPawn)
{

}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("SwitchAbilitySet", IE_Pressed, this, &AARPlayerController::InputSwitchAbilitySet);
	InputComponent->BindAction("InputAbilityManager", IE_Pressed, this, &AARPlayerController::InputShowHideAbilityManager);
	InputComponent->BindAction("InputInventory", IE_Pressed, this, &AARPlayerController::InputShowHideInventory);
}

void AARPlayerController::InputSwitchAbilitySet()
{
}
void AARPlayerController::InputShowHideAbilityManager()
{
	AbilityManager->ShowHideAbilityManager();
}
void AARPlayerController::InputShowHideInventory()
{
	if (AARHUD* MyHUD = Cast<AARHUD>(GetHUD()))
	{
		MyHUD->ShowHideInventory();
	}
}
void AARPlayerController::OnInputAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, FGameplayTag InInputTag)
{
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	UARAbilityBase* Ability = Cast<UARAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	TArray<FGameplayTag> Inputs;
	Inputs.Add(InInputTag);
	AbilityComp->SetAbilityToAction(InAbilityTag, Inputs, FAFOnAbilityReady());
}


/* IIFInventoryInterface */
void AARPlayerController::OnInventoryReplicated(class UIFInventoryComponent* Inventory)
{
	if (Inventory == MainInventory)
	{
		UIComponent->InitializeInventory();
	}
}
/* IIFInventoryInterface */