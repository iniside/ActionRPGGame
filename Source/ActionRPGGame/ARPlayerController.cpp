// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "ARUIAbilityManagerComponent.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"

#include "Weapons/ARWeaponManagerComponent.h"
#include "Abilities/ARAbilityManagerComponent.h"



AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	UIAbilityManagerComponent = ObjectInitializer.CreateDefaultSubobject<UARUIAbilityManagerComponent>(this, "UIAbilityManagerComponent");
	WeaponManager = ObjectInitializer.CreateDefaultSubobject<UARWeaponManagerComponent>(this, "WeaponManager");
	AbilityManager = ObjectInitializer.CreateDefaultSubobject<UARAbilityManagerComponent>(this, "AbilityManager");

	AbilityManager->ComponentTags.Add(TEXT("AbilityManager"));
}

void AARPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	ENetMode NetMode = GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
		if (!ABInt)
			return;

		UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
		if (!AbilityComp)
			return;

		AbilityComp->BindAbilityToAction(InputComponent, InputNextWeapon);
		AbilityComp->BindAbilityToAction(InputComponent, InputPreviousWeapon);
		//WeaponManager->BindInputs();
		//doesn't matter. Internally ability component make sure abilities are instanced on server and replicated back.
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytNextWeapon, InputNextWeapon);
		AbilityComp->AddOnAbilityReadyDelegate(AbilitytNextWeapon, del1);
		AbilityComp->NativeAddAbilityFromTag(AbilitytNextWeapon, nullptr);

		FAFOnAbilityReady del2 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytPreviousWeapon, InputPreviousWeapon);
		AbilityComp->AddOnAbilityReadyDelegate(AbilitytPreviousWeapon, del2);
		AbilityComp->NativeAddAbilityFromTag(AbilitytPreviousWeapon, nullptr);
	}
	//UIAbilityManagerComponent->BindInputs();
}

void AARPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAction("SwitchAbilitySet", IE_Pressed, this, &AARPlayerController::InputSwitchAbilitySet);
	
}

void AARPlayerController::InputSwitchAbilitySet()
{
	UIAbilityManagerComponent->SwitchSet();
}

void AARPlayerController::OnInputAbilityReady(FGameplayTag InAbilityTag, FGameplayTag InInputTag)
{
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	UARAbilityBase* Ability = Cast<UARAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	
	AbilityComp->SetAbilityToAction(InAbilityTag, InInputTag, FAFOnAbilityReady());
}

void AARPlayerController::NextWeapon()
{
	WeaponManager->NextWeapon();
}

void AARPlayerController::PreviousWeapon()
{
	WeaponManager->PreviousWeapon();
}