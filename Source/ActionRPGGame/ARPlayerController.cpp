// Fill out your copyright notice in the Description page of Project Settings.

#include "ARPlayerController.h"
#include "ARUIComponent.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"

#include "Weapons/ARWeaponManagerComponent.h"
#include "Abilities/ARAbilityManagerComponent.h"

#include "UI/ARHUD.h"

AARPlayerController::AARPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIComponent = ObjectInitializer.CreateDefaultSubobject<UARUIComponent>(this, "UIComponent");
	WeaponManager = ObjectInitializer.CreateDefaultSubobject<UARWeaponManagerComponent>(this, "WeaponManager");
	AbilityManager = ObjectInitializer.CreateDefaultSubobject<UARAbilityManagerComponent>(this, "AbilityManager");
	MainInventory = ObjectInitializer.CreateDefaultSubobject<UIFInventoryComponent>(this, "MainInventory");
	MainInventory->SetIsReplicated(true);

	AbilityManager->ComponentTags.Add(TEXT("AbilityManager"));
	bInputBount = false;
}
void AARPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	MainInventory->SetIsReplicated(true);

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
			WeaponManager->BindInputs(InputComponent, AbilityComp);
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

	if (NetMode == ENetMode::NM_DedicatedServer
		|| NetMode == ENetMode::NM_ListenServer)
	{
		ClientPossesed(aPawn);
	}
	
	WeaponManager->POwner = aPawn;

	MainInventory->InitializeInventory();
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

#if WITH_EDITOR
/* Get Screen Percentage */
static const auto CVarScreenPercentage = IConsoleManager::Get().FindTConsoleVariableDataFloat(TEXT("r.SCreenPercentage"));
#endif WITH_EDITOR

float AARPlayerController::GetObjectScreenRadius(AActor* InActor)
{
	float ScreenRadius;
	int32 Width, Height;
	FVector Viewlocation;
	FRotator ViewRotation; // Not Used, but required for Function call
	float CamFOV = 90.0f; //TODO: Replace With Function that returns camera FOV
#if WITH_EDITOR
	float ScreenPerc = CVarScreenPercentage->GetValueOnGameThread() / 100.0f;
#endif WITH_EDITOR

	/* Get the size of the viewport, and the player cameras location. */
	GetViewportSize(Width, Height);
	GetPlayerViewPoint(Viewlocation, ViewRotation);

#if WITH_EDITOR
	/* Factor in Screen Percentage & Quality Settings */
	Width *= ScreenPerc;
	Height *= ScreenPerc;
#endif WITH_EDITOR

	/* Easy Way To Return The Size, Create a vector and scale it. Alternative would be to use FMath::Max3 */
	float SRad = FVector2D(Width, Height).Size();

	/* Get Object Bounds (R) */
	float BoundingRadius = InActor->GetRootComponent()->Bounds.SphereRadius;
	float DistanceToObject = FVector(InActor->GetActorLocation() - Viewlocation).Size();

	/* Get Projected Screen Radius */
	ScreenRadius = FMath::Atan(BoundingRadius / DistanceToObject);
	ScreenRadius *= SRad / FMath::DegreesToRadians(CamFOV);

	return ScreenRadius;
}
void AARPlayerController::GetObjectBoundSphere(float Distance, AActor* InActor, FVector& Origin, float& Radius, float& Scale
	, float& SphereRadius)
{
	const FMinimalViewInfo& ViewInfo = PlayerCameraManager->GetLastFrameCameraCachePOV();
	FMatrix Proj = ViewInfo.CalculateProjectionMatrix();
	const float ScreenMultiple = FMath::Max(0.5f * Proj.M[0][0], 0.5f * Proj.M[1][1]);
	if (ACharacter* Character = Cast<ACharacter>(InActor))
	{
		Origin = Character->GetMesh()->Bounds.Origin;
		Radius = Character->GetMesh()->Bounds.SphereRadius;
		Scale = ScreenMultiple;
		SphereRadius = ScreenMultiple * Radius / FMath::Max(1.0f, Distance);
		return;
	}
	
	Scale = ScreenMultiple;
	Origin = InActor->GetRootComponent()->Bounds.Origin;
	Radius = InActor->GetRootComponent()->Bounds.SphereRadius;
	SphereRadius = ScreenMultiple * Radius / FMath::Max(1.0f, Distance);
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