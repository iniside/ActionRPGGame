// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "ActionRPGGame.h"

//#include "GAAbilitiesComponent.h"
#include "Items/ARItemInfo.h"
#include "Widgets/GISContainerBaseWidget.h"
#include "Items/GSEquipmentComponent.h"
#include "Attributes/GAAttributesBase.h"

#include "Items/GSEquipmentComponent.h"
#include "Weapons/GSWeaponEquipmentComponent.h"
#include "Components/GSActiveActionsComponent.h"

#include "Items/GSItemWeaponInfo.h"

#include "GSGlobalTypes.h"

//#include "Net/UnrealNetwork.h"
//#include "Engine/ActorChannel.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GSPlayerController.h"
#include "ARPlayerController.h"
#include "ARDebugHUD.h"
#include "ARCharacterMovementComponent.h"
#include "ARCharacter.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

AARCharacter::AARCharacter(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer.SetDefaultSubobjectClass<UARCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
    // set our turn rates for input
    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = false;
	PrimaryActorTick.bAllowTickOnDedicatedServer = true;
    bReplicates = true;

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->NavAgentProps.bCanJump = true;
    GetCharacterMovement()->NavAgentProps.bCanWalk = true;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = ObjectInitializer.CreateDefaultSubobject<USpringArmComponent>(this, TEXT("CameraBoom"));
    CameraBoom->AttachToComponent(GetCapsuleComponent(), FAttachmentTransformRules::KeepRelativeTransform);//(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, "head");
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->AttachToComponent(GetMesh(),FAttachmentTransformRules::KeepRelativeTransform, "headSocket");
    // Create a follow camera
    FollowCamera = ObjectInitializer.CreateDefaultSubobject<UCameraComponent>(this, TEXT("FollowCamera"));
    FollowCamera->AttachToComponent(CameraBoom, FAttachmentTransformRules::KeepRelativeTransform, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm


    //testing out multi commponent interaction

    Attributes = ObjectInitializer.CreateDefaultSubobject<UGAAbilitiesComponent>(this, TEXT("Attributes"));
    Attributes->SetIsReplicated(true);
    Attributes->SetNetAddressable();
	
	
    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
    // are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AARCharacter::BeginPlay()
{
    Super::BeginPlay();

    Attributes->SetIsReplicated(true);
    Attributes->SetNetAddressable();
	//Attributes->SetComponentTickEnabled(true);
	//Attributes->RegisterComponent();
	//Attributes->RegisterAllComponentTickFunctions(true);
}
bool AARCharacter::GetIsPivotChanging() const
{
	return bIsPivotChanging;
}

float AARCharacter::GetMovementDot() const
{
	return MovementDot;
}

FVector AARCharacter::GetAccelerationLocalDir() const
{
	return AccelerationLocalDir;
}

FVector AARCharacter::GetAccelerationWorldDir() const
{
	return AccelerationWorldDir;
}

FVector AARCharacter::GetVelocityLocalDir() const
{
	return VelocityLocalDir;
}

FVector AARCharacter::GetVelocityWorldDir() const
{
	return VelocityWorldDir;
}
FRotator AARCharacter::GetAccelerationRotator() const
{
	return AccelerationRotator;
}
FRotator AARCharacter::GetVelocityRotator() const
{
	return VelocityRotator;
}
bool bSphereDrawn = false;
bool bSphereDrawn2 = false;
void AARCharacter::Tick(float DeltaTime)
{
	if (Attributes)
	{
		Attributes->Update();
	}
	Super::Tick(DeltaTime);
	FVector CurrentLocation = GetActorLocation();
	const bool bZeroAcceleration = GetCharacterMovement()->GetCurrentAcceleration().IsZero();
	FVector CurretVelocity = GetCharacterMovement()->Velocity;
	FVector CurretAccel = GetCharacterMovement()->GetCurrentAcceleration();
	FVector AccelArrowEnd = CurrentLocation + (CurretAccel.GetSafeNormal() * 50.0f);
	FVector VelArrowEnd = CurrentLocation + (CurretVelocity.GetSafeNormal() * 50.0f);
	UKismetSystemLibrary::DrawDebugArrow(this, CurrentLocation, AccelArrowEnd, 5, FLinearColor::Red, DeltaTime*1.5f, 2);
	UKismetSystemLibrary::DrawDebugArrow(this, CurrentLocation, VelArrowEnd, 5, FLinearColor::Blue, DeltaTime*1.5f, 2);
	FVector CapsuleForward = CurrentLocation + (GetCapsuleComponent()->GetForwardVector() * 50.0f);
	UKismetSystemLibrary::DrawDebugArrow(this, CurrentLocation, CapsuleForward, 5, FLinearColor::Green, DeltaTime*1.5f, 2);
	FVector ControlForward = CurrentLocation + (GetControlRotation().Vector() * 50.0f);
	UKismetSystemLibrary::DrawDebugArrow(this, CurrentLocation, ControlForward, 5, FLinearColor::Yellow, DeltaTime*1.5f, 2);

	FTransform VelocityTransform = GetTransform();
	FVector LocalAcc = VelocityTransform.InverseTransformVector(CurretAccel.GetSafeNormal());
	FVector LocalVel = VelocityTransform.InverseTransformVector(CurretVelocity.GetSafeNormal());
	FVector IsNearlyZero = LocalAcc.GetAbs() - LocalVel.GetAbs();
	FVector Sign = LocalAcc.GetSignVector();
	bool test = IsNearlyZero.IsNearlyZero(0.1);
	const float FrictionFactor = FMath::Max(0.f, GetCharacterMovement()->BrakingFrictionFactor);
	float Friction = FMath::Max(0.f, GetCharacterMovement()->BrakingFriction * FrictionFactor);
	float DotProduct = FVector::DotProduct(CurretAccel.GetSafeNormal(), CurretVelocity.GetSafeNormal());
	
	MovementDot = DotProduct;
	AccelerationLocalDir = LocalAcc;
	AccelerationWorldDir = CurretAccel.GetSafeNormal();
	VelocityLocalDir = LocalVel;
	VelocityWorldDir = CurretVelocity.GetSafeNormal();
	AccelerationRotator = LocalAcc.ToOrientationRotator();
	VelocityRotator = LocalVel.ToOrientationRotator();
	if (DotProduct < -0.7)
	{
		bIsPivotChanging = true;
	}
	else
	{
		bIsPivotChanging = false;
	}
	if (DotProduct != 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character.Tick DotProduct: %f"), DotProduct);
	}
	if (!test)
	{
		float Distanc = GetCharacterMovement()->Velocity.SizeSquared() / (2 * Friction * -GetWorld()->GetGravityZ());
		FVector Forward = GetCharacterMovement()->Velocity;
		Forward.Normalize();
		FVector StopLocation = CurrentLocation + (Forward * (Distanc));
		//UE_LOG(LogTemp, Warning, TEXT("Character.Tick Stopping Distanc: %f / Position: X: %f Y: %f Z: %f Friction: %f FrictionFactor: %f BrakingFriction: %f GetGravityZ: %f, Velocity: %f, CurretAccelerration: %f"), Distanc, StopLocation.X, StopLocation.Y, StopLocation.Z, Friction, FrictionFactor, GetCharacterMovement()->BrakingFriction, GetWorld()->GetGravityZ(), GetCharacterMovement()->Velocity.SizeSquared(), GetCharacterMovement()->GetCurrentAcceleration().SizeSquared());
		if (!bSphereDrawn2)
		{
			UKismetSystemLibrary::DrawDebugSphere(this, StopLocation, 10, 16, FColor::Magenta, 15, 2);
			bSphereDrawn2 = true;
		}
	}
	else
	{
		bSphereDrawn2 = false;
	}
	float VelSpeeDot = FVector::DotProduct(CurretAccel.GetSafeNormal(), CurretVelocity.GetSafeNormal());
	float StrafeDir = FVector::DotProduct(GetActorForwardVector().GetSafeNormal(), CurretAccel.GetSafeNormal());
	float angle = FMath::Atan2(LocalVel.Y, LocalVel.X);
	EightDirection = FMath::RoundToInt(8 * angle / (2 * PI) + 8) % 8;
	MoveDirection = EightDirection;
	Angle = FMath::RadiansToDegrees(angle);
	
	switch (EightDirection)
	{
	case 0:
		MoveDirection = 0;
		break;
	case 1:
		MoveDirection = 0;
		break;
	case 2:
		MoveDirection = 1;
		break;
	case 3:
		MoveDirection = 3;
		break;
	case 4:
		MoveDirection = 3;
		break;
	case 5:
		MoveDirection = 3;
		break;
	case 6:
		MoveDirection = 2;
		break;
	case 7:
		MoveDirection = 0;
		break;
	default:
		MoveDirection = 0;
		break;
	}

	if (GetGamePlayerController())
	{
		AHUD* hud = GetGamePlayerController()->GetHUD();
		AARDebugHUD* HUD = Cast<AARDebugHUD>(hud);
		if (HUD)
		{
			HUD->MoveDirection = EightDirection;
			HUD->VelSpeedDot = VelSpeeDot;
			HUD->StrafeDir = StrafeDir;
			HUD->AccelX = LocalAcc.X;
			HUD->AccelY = LocalAcc.Y;

			HUD->VelocityX = LocalVel.X;
			HUD->VelocityY = LocalVel.Y;
			HUD->Angle = FMath::RadiansToDegrees(angle);
		}
	}
	
	if (!bZeroAcceleration)
	{
		bSphereDrawn = false;
	}
	//UE_LOG(LogTemp, Warning, TEXT("Character.Tick CurretAccelerration: %f"), GetCharacterMovement()->GetCurrentAcceleration().SizeSquared());
	if (bZeroAcceleration)
	{
		//time = (0 - Velocity) /decel;
		float time = (0 - GetCharacterMovement()->Velocity.Size()) / GetCharacterMovement()->GetMaxBrakingDeceleration();

		float Distanc = GetCharacterMovement()->Velocity.SizeSquared() / (2 * Friction * -GetWorld()->GetGravityZ());
		FVector Forward = GetCharacterMovement()->Velocity;
		Forward.Normalize();
		FVector StopLocation = CurrentLocation + (Forward * (2 * Distanc));
		//UE_LOG(LogTemp, Warning, TEXT("Character.Tick Stopping Distanc: %f / Position: X: %f Y: %f Z: %f Friction: %f FrictionFactor: %f BrakingFriction: %f GetGravityZ: %f, Velocity: %f, CurretAccelerration: %f"), Distanc, StopLocation.X, StopLocation.Y, StopLocation.Z, Friction, FrictionFactor, GetCharacterMovement()->BrakingFriction, GetWorld()->GetGravityZ(), GetCharacterMovement()->Velocity.SizeSquared(), GetCharacterMovement()->GetCurrentAcceleration().SizeSquared());
		if (!bSphereDrawn)
		{
			if (test)
			{
				UKismetSystemLibrary::DrawDebugSphere(this, StopLocation, 10, 16, FColor::Red, 15, 2);
				bSphereDrawn = true;
			}
		}
	}
}
void AARCharacter::OnRep_Controller()
{
    Super::OnRep_Controller();
    ARPController = Cast<AARPlayerController>(Controller);
    Attributes->PCOwner = ARPController;
    if (ARPController)
    {
    }
    OnPostControllerRep(ARPController);
}
void AARCharacter::PossessedBy(AController* NewController)
{
    ARPController = Cast<AARPlayerController>(NewController);
    if ((GetNetMode() == ENetMode::NM_Standalone
        || GetNetMode() == ENetMode::NM_Client)
        && ARPController)
    {
        Attributes->PCOwner = ARPController;
    }
    Super::PossessedBy(NewController);
}
/** IIGAAbilities Begin */
class UGAAttributesBase* AARCharacter::GetAttributes()
{
    return Attributes->DefaultAttributes;
}

class UGAAbilitiesComponent* AARCharacter::GetAbilityComp()
{
    return Attributes;
}

float AARCharacter::GetAttributeValue(FGAAttribute AttributeIn) const
{
    return Attributes->DefaultAttributes->GetFloatValue(AttributeIn);
}
void AARCharacter::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	, FGAEffectProperty& InProperty)
{
	GetAttributes()->ModifyAttribute(ModIn, HandleIn, InProperty);
}
FAFAttributeBase* AARCharacter::GetAttribute(FGAAttribute AttributeIn)
{
	return GetAttributes()->GetAttribute(AttributeIn);
}
void AARCharacter::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod)
{
	GetAttributes()->RemoveBonus(AttributeIn, HandleIn, HandleIn.GetAttributeMod());
}
FGAEffectHandle AARCharacter::ApplyEffectToTarget(const FGAEffect& EffectIn, const FGAEffectHandle& HandleIn,
	FGAEffectProperty& InProperty)
{
	return GetAbilityComp()->ApplyEffectToTarget(EffectIn, HandleIn, InProperty);
};
void AARCharacter::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	GetAbilityComp()->RemoveTagContainer(TagsIn);
}
float AARCharacter::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return 0;
}
/** IIGAAbilities End */

/** IIGTSocket Begin */
//FVector AARCharacter::GetSocketLocation(FName SocketNameIn)
//{
//	FVector SocketLocation = FVector::ZeroVector;
//	if (GetMesh())
//		SocketLocation = GetMesh()->GetSocketLocation(SocketNameIn);
//	return SocketLocation;
//}
/* IIGTSocket End **/
//////////////////////////////////////////////////////////////////////////
// Input
void AARCharacter::InputGetNextLeftWeapon()
{
    WeaponsEquipment->SwapWeaponSet();
    //ActiveActions->SetWeaponFrom(WeaponsEquipment, 0, 1, 0, EGSWeaponHand::Left);
}
void AARCharacter::InputGetNextRightWeapon()
{
    //ActiveActions->SetWeaponFrom(RightWeaponsEquipment, 0, 2, 0, EGSWeaponHand::Right);
}

void AARCharacter::InputUseLeftWeaponPressed()
{
    WeaponsEquipment->InputMainWeaponPressed();
}
void AARCharacter::InputUseLeftWeaponReleased()
{
    WeaponsEquipment->InputMainWeaponReleased();
}

void AARCharacter::InputUseRightWeaponPressed()
{
    WeaponsEquipment->InputOffWeaponPressed();
}
void AARCharacter::InputUseRightWeaponReleased()
{
    WeaponsEquipment->InputOffWeaponReleased();
}


void AARCharacter::InputReloadWeapon()
{

}
void AARCharacter::InputAbilityPressed()
{

}
void AARCharacter::InputAbilityReleased()
{

}
void AARCharacter::SetupPlayerInputComponent(class UInputComponent* InInputComponent)
{
    // Set up gameplay key bindings
    check(InInputComponent);
	InInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	InInputComponent->BindAxis("MoveForward", this, &AARCharacter::MoveForward);
	InInputComponent->BindAxis("MoveRight", this, &AARCharacter::MoveRight);

	InInputComponent->BindAction("ShowInventory", IE_Pressed, this, &AARCharacter::ShowHideInventory);
	InInputComponent->BindAction("ShowHideAbilityBook", IE_Pressed, this, &AARCharacter::ShowHideAbilityBook);

	InInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AARCharacter::InputActionBarPressed<1, 0>);
	InInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AARCharacter::InputActionBarReleased<1, 0>);

	InInputComponent->BindAction("RightMouseButton", IE_Pressed, this, &AARCharacter::InputActionBarPressed<1, 1>);
	InInputComponent->BindAction("RightMouseButton", IE_Released, this, &AARCharacter::InputActionBarReleased<1, 1>);

	InInputComponent->BindAction("ActionButtonTab0Slot0", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 0>);
	InInputComponent->BindAction("ActionButtonTab0Slot1", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 1>);
	InInputComponent->BindAction("ActionButtonTab0Slot2", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 2>);
	InInputComponent->BindAction("ActionButtonTab0Slot3", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 3>);
	InInputComponent->BindAction("ActionButtonTab0Slot4", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 4>);
	InInputComponent->BindAction("ActionButtonTab0Slot5", IE_Pressed, this, &AARCharacter::InputActionBarPressed<0, 5>);

	InInputComponent->BindAction("ActionButtonTab0Slot0", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 0>);
	InInputComponent->BindAction("ActionButtonTab0Slot1", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 1>);
	InInputComponent->BindAction("ActionButtonTab0Slot2", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 2>);
	InInputComponent->BindAction("ActionButtonTab0Slot3", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 3>);
	InInputComponent->BindAction("ActionButtonTab0Slot4", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 4>);
	InInputComponent->BindAction("ActionButtonTab0Slot5", IE_Released, this, &AARCharacter::InputActionBarReleased<0, 5>);

	InInputComponent->BindAction("InputGetNextLeftWeapon", IE_Pressed, this, &AARCharacter::InputGetNextLeftWeapon);
	InInputComponent->BindAction("InputGetNextRightWeapon", IE_Pressed, this, &AARCharacter::InputGetNextRightWeapon);

	InInputComponent->BindAction("InputUseLeftWeapon", IE_Pressed, this, &AARCharacter::InputUseLeftWeaponPressed);
	InInputComponent->BindAction("InputUseLeftWeapon", IE_Released, this, &AARCharacter::InputUseLeftWeaponReleased);

	InInputComponent->BindAction("InputUseRighttWeapon", IE_Pressed, this, &AARCharacter::InputUseRightWeaponPressed);
	InInputComponent->BindAction("InputUseRighttWeapon", IE_Released, this, &AARCharacter::InputUseRightWeaponReleased);

	InInputComponent->BindAction("InputReloadWeapon", IE_Released, this, &AARCharacter::InputReloadWeapon);

	InInputComponent->BindAction("InputUseAbility", IE_Pressed, this, &AARCharacter::InputAbilityPressed);
	InInputComponent->BindAction("InputUseAbility", IE_Released, this, &AARCharacter::InputAbilityReleased);

	InInputComponent->BindAction("SwapActionBars", IE_Pressed, this, &AARCharacter::InputSwapActionBars);
	InInputComponent->BindAction("ShowHideEditableActionBars", IE_Pressed, this, &AARCharacter::ShowHideEditableHotbars);
    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InInputComponent->BindAxis("TurnRate", this, &AARCharacter::TurnAtRate);
	InInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InInputComponent->BindAxis("LookUpRate", this, &AARCharacter::LookUpAtRate);

    // handle touch devices
	InInputComponent->BindTouch(IE_Pressed, this, &AARCharacter::TouchStarted);
	InInputComponent->BindTouch(IE_Released, this, &AARCharacter::TouchStopped);


    //InputComponent->BindAction("ActionButton1", IE_Pressed, this, &AARCharacter::InputActionBar<0>);
}
void AARCharacter::ActivateAbility()
{

}
void AARCharacter::InputSwapActionBars()
{
    //ActionBar->CopyItemsFromOtherInventoryTab(StaticActionBar, 0);
    ////ActionBar->CopyItemsToTargetTabFromLinkedTabs();
}
void AARCharacter::ShowHideEditableHotbars()
{
}
void AARCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
    // jump, but only on the first touch
    if (FingerIndex == ETouchIndex::Touch1)
    {
        Jump();
    }
}

void AARCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
    if (FingerIndex == ETouchIndex::Touch1)
    {
        StopJumping();
    }
}

void AARCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AARCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AARCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        FRotator Rotation;// = GetBaseAimRotation();

        FVector Location; //not used, just need for below.
        Controller->GetPlayerViewPoint(Location, Rotation);
        Rotation.Normalize();
        FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		DirectionX = Direction;
        AddMovementInput(Direction, Value);
        if (Value > 0)
            bIsMovingForward = true;
        else if (Value < 0)
            bIsMovingBackward = true;
    }
    else
    {
        bIsMovingBackward = false;
        bIsMovingForward = false;
    }
}

void AARCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        FRotator Rotation;
        FVector Location; //not used, just need for below.
        Controller->GetPlayerViewPoint(Location, Rotation);
        Rotation.Normalize();
        FRotator YawRotation(0, Rotation.Yaw, 0);
        const FVector Direction = FRotationMatrix(YawRotation).GetScaledAxis(EAxis::Y);
		DirectionY = Direction;
        // add movement in that direction
        AddMovementInput(Direction, Value);
        if (Value > 0)
            bIsStrafingRight = true;
        else if (Value < 0)
            bIsStrafingLeft = true;
    }
    else
    {
        bIsStrafingRight = false;
        bIsStrafingLeft = false;
    }
}

void AARCharacter::ShowHideInventory()
{
    //if (Inventory && Inventory->InventoryContainer)
    //{
    //	if (Inventory->InventoryContainer->GetVisibility() != ESlateVisibility::SelfHitTestInvisible)
    //	{
    //		Inventory->InventoryContainer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    //		if (Inventory->InventoryContainer->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    //			GEngine->AddOnScreenDebugMessage(0, 4, FColor::Red, "Widget Is SelfHitTestInvisible");
    //		else
    //			GEngine->AddOnScreenDebugMessage(0, 4, FColor::Red, "something is fucked up.");
    //		
    //	}
    //	else if (Inventory->InventoryContainer->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    //	{
    //		Inventory->InventoryContainer->SetVisibility(ESlateVisibility::Collapsed);
    //		if (Inventory->InventoryContainer->GetVisibility() == ESlateVisibility::Collapsed)
    //			GEngine->AddOnScreenDebugMessage(0, 4, FColor::Red, "Widget Is Collapsed");
    //		else
    //			GEngine->AddOnScreenDebugMessage(0, 4, FColor::Red, "something is fucked up.");
    //	}
    //}
}

void AARCharacter::ShowHideAbilityBook()
{
    //if (AbilityBook && AbilityBook->InventoryContainer)
    //{
    //	//AbilityBook->ChangeTabVisibility(0);
    //	//if (AbilityBook->InventoryContainer->GetVisibility() == ESlateVisibility::Hidden)
    //	//{
    //	//	AbilityBook->InventoryContainer->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    //	//}
    //	//else if (AbilityBook->InventoryContainer->GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
    //	//{
    //	//	AbilityBook->InventoryContainer->SetVisibility(ESlateVisibility::Hidden);
    //	//}
    //}
}
APlayerController* AARCharacter::GetGamePlayerController()
{
    return ARPController;
}

float AARCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
    ReceiveAnyDamage(DamageAmount, nullptr, EventInstigator, DamageCauser);
  //  OnTakeAnyDamage.Broadcast(DamageAmount, nullptr, EventInstigator, DamageCauser);
    OnCharacterDied();
    return 0;
}