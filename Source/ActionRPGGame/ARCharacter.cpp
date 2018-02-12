// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "ARCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Weapons/ARWeaponBase.h"

#include "Weapons/ARWeaponPawnManagerComponent.h"
#include "ARPlayerController.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

const FName WeaponSocket::HolsteredRightWeapon = TEXT("spine_RightWeaponSocket");
const FName WeaponSocket::HolsteredLeftWeapon = TEXT("spine_LeftWeaponSocket");
const FName WeaponSocket::EquipedMainWeapon = TEXT("hand_rSocket");

AARCharacter::AARCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Abilities = CreateDefaultSubobject<UAFAbilityComponent>(TEXT("Abilities"));
	Abilities->SetIsReplicated(true);
	EffectsComponent = CreateDefaultSubobject<UAFEffectsComponent>(TEXT("EffectsComponent"));
	EffectsComponent->SetIsReplicated(true);
	
	FollowCamera->TransformUpdated.AddUObject(this, &AARCharacter::OnCameraTransformUpdate);

	Weapons = CreateDefaultSubobject<UARWeaponPawnManagerComponent>(TEXT("Weapons"));

	WeaponHolsteredRight = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponHolsteredRight"));
	//HolsteredRightWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform);
	WeaponHolsteredRight->SetupAttachment(GetMesh(), WeaponSocket::HolsteredRightWeapon);
	
	
	WeaponHolsteredLeft = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponHolsteredLeft"));
	//HolsteredLeftWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	WeaponHolsteredLeft->SetupAttachment(GetMesh(), WeaponSocket::HolsteredLeftWeapon);
	
	
	HolsteredBackDown = CreateDefaultSubobject<UChildActorComponent>(TEXT("HolsteredBackDown"));
	//HolsteredBackDownWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	HolsteredBackDown->SetupAttachment(GetMesh());
	
	
	WeaponHolsteredSideLeft = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponHolsteredSideLeft"));
	//HolsteredSideLeftWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	WeaponHolsteredSideLeft->SetupAttachment(GetMesh());
	
	
	WeaponEquipedMain = CreateDefaultSubobject<UChildActorComponent>(TEXT("WeaponEquipedMain"));
	//EquipedMainWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	WeaponEquipedMain->SetupAttachment(GetMesh(), WeaponSocket::EquipedMainWeapon);
	


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AARCharacter::BeginPlay()
{
	Super::BeginPlay();

	Weapons->SetPOwner(this);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AARCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AARCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AARCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AARCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AARCharacter::LookUpAtRate);

	Abilities->BindInputs(PlayerInputComponent);
}

void AARCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
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
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AARCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
/* IAFAbilityInterface- BEGIN */

class UAFAbilityComponent* AARCharacter::GetAbilityComp() 
{ 
	return Abilities;
};

class UAFEffectsComponent* AARCharacter::GetEffectsComponent()
{
	return EffectsComponent;
}
class UAFEffectsComponent* AARCharacter::NativeGetEffectsComponent() const
{
	return EffectsComponent;
}
float AARCharacter::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return Abilities->GetAttributeValue(AttributeIn);
}

void AARCharacter::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
	struct FGAEffectProperty& InProperty)
{
	Abilities->ModifyAttribute(ModIn, HandleIn, InProperty);
}

FAFAttributeBase* AARCharacter::GetAttribute(FGAAttribute AttributeIn) 
{ 
	return Abilities->GetAttribute(AttributeIn);
};

void AARCharacter::RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod)
{
	Abilities->RemoveBonus(AttributeIn, HandleIn, InMod);
}

float AARCharacter::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return Abilities->NativeGetAttributeValue(AttributeIn);
}
/* IAFAbilityInterface- END */

void AARCharacter::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AARCharacter, CameraTransform, COND_SkipOwner);
}

void AARCharacter::OnCameraTransformUpdate(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	if (GetNetMode() == ENetMode::NM_Standalone
		|| Role == ENetRole::ROLE_Authority
		|| Role == ENetRole::ROLE_AutonomousProxy)
	{
		CameraTransform.ForwardVector = FollowCamera->GetForwardVector();
		CameraTransform.Location = FollowCamera->GetComponentLocation();
	}
}

class AARWeaponBase* AARCharacter::GetMainWeapon() const
{
	return GetMainWeapon<AARWeaponBase>();
}

USkeletalMeshComponent* AARCharacter::GetMainWeaponMesh() const
{
	if (!GetEquipedMainWeapon()->GetChildActor())
		return nullptr;

	return GetMainWeapon<AARWeaponBase>()->GetMesh();
}

FVector AARCharacter::GetMainWeaponSocket(const FName& Socket) const
{
	USkeletalMeshComponent* Component = GetMainWeaponMesh();
	if (!Component)
		return GetMesh()->GetSocketLocation(TEXT("headSocket"));

	return Component->GetSocketLocation(Socket);
}