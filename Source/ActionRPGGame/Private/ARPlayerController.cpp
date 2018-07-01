// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"
#include "ARCharacter.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Components/CapsuleComponent.h"


#include "UI/ARHUD.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	MainInventory = ObjectInitializer.CreateDefaultSubobject<UIFInventoryComponent>(this, "MainInventory");
	MainInventory->SetIsReplicated(true);

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
			AARCharacter* Character = Cast<AARCharacter>(GetPawn());
			
			bInputBount = true;
		}
		//doesn't matter. Internally ability component make sure abilities are instanced on server and replicated back.
		if (!AbilitytNextWeapon.IsNull())
		{
			InputNextWeaponSpecHandle = FAFAbilitySpecHandle::GenerateHandle();
			FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady);
			AbilityComp->AddOnAbilityReadyDelegate(InputNextWeaponSpecHandle, del1);

			AbilityComp->NativeAddAbility(AbilitytNextWeapon, InputNextWeaponSpecHandle);
		}
		/*if (!AbilitytPreviousWeapon.IsNull())
		{
			FAFOnAbilityReady del2 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytPreviousWeapon, InputPreviousWeapon);
			AbilityComp->AddOnAbilityReadyDelegate(AbilitytPreviousWeapon, del2);
			TArray<FGameplayTag> PrevWeap;
			PrevWeap.Add(InputPreviousWeapon);
			AbilityComp->NativeAddAbility(AbilitytPreviousWeapon);
		}
		if (!AbilitytHolstersWeapon.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, AbilitytHolstersWeapon, InputHolsterWeapon);
			AbilityComp->AddOnAbilityReadyDelegate(AbilitytHolstersWeapon, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputHolsterWeapon);
			AbilityComp->NativeAddAbility(AbilitytHolstersWeapon);
		}

		if (!SetAbilityGroup01.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, SetAbilityGroup01, InputSetAbilityGroup01);
			AbilityComp->AddOnAbilityReadyDelegate(SetAbilityGroup01, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputSetAbilityGroup01);
			AbilityComp->NativeAddAbility(SetAbilityGroup01);
		}
		if (!SetAbilityGroup02.IsNull())
		{
			FAFOnAbilityReady del3 = FAFOnAbilityReady::CreateUObject(this, &AARPlayerController::OnInputAbilityReady, SetAbilityGroup02, InputSetAbilityGroup02);
			AbilityComp->AddOnAbilityReadyDelegate(SetAbilityGroup02, del3);
			TArray<FGameplayTag> HolsterInput;
			HolsterInput.Add(InputSetAbilityGroup02);
			AbilityComp->NativeAddAbility(SetAbilityGroup02);
		}*/

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
}
void AARPlayerController::InputShowHideInventory()
{
	if (AARHUD* MyHUD = Cast<AARHUD>(GetHUD()))
	{
		MyHUD->ShowHideInventory();
	}
}
void AARPlayerController::OnInputAbilityReady(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	InputNextWeaponSpecHandle = ServerHandle;

	TArray<uint8> InputIds;
	InputIds.Add(static_cast<uint8>(AbilityInput::NextWeapon));
	AbilityComp->BindAbilityToInputIDs(ServerHandle, InputIds);
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

float AARPlayerController::ComputeBoundsScreenSize(UCapsuleComponent* InTarget)
{
	ULocalPlayer const* const LP = GetLocalPlayer();
	FVector4 ViewOrigin;
	FMatrix ProjMatrix;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			ViewOrigin = ProjectionData.ViewOrigin;
			ProjMatrix = ProjectionData.ProjectionMatrix;
		}
	}
	const float Dist = FVector::Dist(InTarget->Bounds.Origin, ViewOrigin);

	// Get projection multiple accounting for view scaling.
	const float ScreenMultiple = FMath::Max(0.5f * ProjMatrix.M[0][0], 0.5f * ProjMatrix.M[1][1]);

	// Calculate screen-space projected radius
	const float ScreenRadius = ScreenMultiple * InTarget->Bounds.SphereRadius / FMath::Max(1.0f, Dist);

	// For clarity, we end up comparing the diameter
	return ScreenRadius * 2.0f;
}