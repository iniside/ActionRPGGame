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
#include "UI/ARUIComponent.h"

#include "Weapons/ARWeaponInventoryComponent.h"
#include "ARCharacterMovementComponent.h"
#include "ARPlayerController.h"

//////////////////////////////////////////////////////////////////////////
// AARCharacter

const FName WeaponSocket::HolsteredRightWeapon = TEXT("spine_RightWeaponSocket");
const FName WeaponSocket::HolsteredLeftWeapon = TEXT("spine_LeftWeaponSocket");
const FName WeaponSocket::EquipedMainWeapon = TEXT("hand_rSocket");

AARCharacter::AARCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UARCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 5.f;
	BaseLookUpRate = 25.f;
	bStopDistancePredicted = false;
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 180.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	GetCharacterMovement()->MaxAcceleration = 200.0f;
	GetCharacterMovement()->BrakingFriction = 1.0;
	GetCharacterMovement()->bUseSeparateBrakingFriction = true;
	GetCharacterMovement()->Mass = 80.0f;
	GetCharacterMovement()->GroundFriction = 1.0f;
	GetCharacterMovement()->MaxWalkSpeed = 270.0f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 150.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 150.0f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("headSocket"));
	
	CameraBoom->TargetArmLength = 275; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0, 25, 65);
	CameraBoom->TargetOffset = FVector(0, 0, -45);
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->CameraLagSpeed = 8;
	CameraBoom->CameraRotationLagSpeed = 7;
	CameraBoom->CameraLagMaxDistance = 10;
	CameraBoom->ProbeSize = 0.05f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	Abilities = CreateDefaultSubobject<UAFAbilityComponent>(TEXT("Abilities"));
	Abilities->SetIsReplicated(true);
	EffectsComponent = CreateDefaultSubobject<UAFEffectsComponent>(TEXT("EffectsComponent"));
	EffectsComponent->SetIsReplicated(true);
	
	WeaponInventory = ObjectInitializer.CreateDefaultSubobject<UARWeaponInventoryComponent>(this, TEXT("WeaponInventory"));
	WeaponInventory->SetIsReplicated(true);

	FollowCamera->TransformUpdated.AddUObject(this, &AARCharacter::OnCameraTransformUpdate);

	Head = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Head"));
	Head->SetupAttachment(GetMesh());
	Head->SetMasterPoseComponent(GetMesh());

	Shoulders = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoulders"));
	Shoulders->SetupAttachment(GetMesh());
	Shoulders->SetMasterPoseComponent(GetMesh());

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(GetMesh());
	Arms->SetMasterPoseComponent(GetMesh());

	Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	Hands->SetupAttachment(GetMesh());
	Hands->SetMasterPoseComponent(GetMesh());

	Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	Torso->SetupAttachment(GetMesh());
	Torso->SetMasterPoseComponent(GetMesh());

	Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	Legs->SetupAttachment(GetMesh());
	Legs->SetMasterPoseComponent(GetMesh());

	Feets = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feets"));
	Feets->SetupAttachment(GetMesh());
	Feets->SetMasterPoseComponent(GetMesh());

	Backpack = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
	Backpack->SetupAttachment(GetMesh());
	Backpack->SetMasterPoseComponent(GetMesh());

	LegsCloth = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsCloth"));
	LegsCloth->SetupAttachment(GetMesh());
	//LegsCloth->SetMasterPoseComponent(GetMesh());

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
	
	bUseControllerRotationYaw = true;
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AARCharacter::BeginPlay()
{
	Super::BeginPlay();
	WeaponInventory->SetIsReplicated(true);
	WeaponInventory->InitializeWeapons(this);
}

FString DirToString(EFourCardinalDirection dir)
{
	switch (dir)
	{
	case EFourCardinalDirection::N:
		return "N";
	case EFourCardinalDirection::E:
		return "E";
	case EFourCardinalDirection::S:
		return "S";
	case EFourCardinalDirection::W:
		return "W";
	default:
		break;
	}
	return "Invalid";
}

float AARCharacter::GetAnimOrient()
{
	return CurrentOrient;
}
float AARCharacter::GetAnimOrientN() { return OrientN; };
float AARCharacter::GetAnimOrientE() { return OrientE; };
float AARCharacter::GetAnimOrientS() { return OrientS; };
float AARCharacter::GetAnimOrientW() { return OrientW; };

EFCardinalDirection AARCharacter::GetCardianlDirection()
{
	int32 dir =  static_cast<int32>(FourDirections);

	return static_cast<EFCardinalDirection>(dir);
}
void AARCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UCharacterMovementComponent* CMC = GetCharacterMovement();

	if (!CMC)
		return;
	FVector CharLocation = GetActorLocation();

	FVector CurrentAcceleration = CMC->GetCurrentAcceleration();
	FVector CurrentVelocity = CMC->Velocity;
	
	FVector AccelerationDirection = CurrentAcceleration.GetSafeNormal();
	FVector LineEnd = (AccelerationDirection * 80.0f) + GetActorLocation();

	//::DrawDebugLine(GetWorld(), GetActorLocation(), LineEnd, FColor::Red, false, -1.0f, 0, 10);
	
	FVector VelocityDirection = CurrentVelocity.GetSafeNormal();

	float Vel = (CurrentVelocity.Size() / CMC->GetMaxSpeed()) * 100;

	FVector VelocityEnd = (VelocityDirection * Vel) + GetActorLocation();

	//::DrawDebugLine(GetWorld(), GetActorLocation()+FVector(0,0,10), VelocityEnd + FVector(0, 0, 10), FColor::Blue, false, -1.0f, 0, 10);
	
	FTransform Transform = GetTransform();
	FVector LocalAcceleration = Transform.InverseTransformVectorNoScale(AccelerationDirection);
	FVector LocalVelocity = Transform.InverseTransformVectorNoScale(VelocityDirection);

	FQuat QRot = LocalAcceleration.ToOrientationQuat();
	FRotator Rot = FRotator(QRot);
	float Angle = Rot.Yaw;// FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	float Angle2 = (FMath::RoundToInt(Angle) + 360) % 360;
	FVector Right = GetActorRightVector();
	FVector Forward = GetActorForwardVector();
	

	FQuat QAngle = FQuat::FindBetweenNormals(Forward, LocalVelocity);
	
	FRotator RAngle(QAngle);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 145), "RAngle: " + FString::FormatAsNumber(RAngle.Yaw), nullptr, FColor::Red, 0, true);


	FVector V1 = (Forward + Right).GetSafeNormal2D();
	FVector V2 = (Forward + (Right * (-1))).GetSafeNormal2D();
	

	int32 Octant = FMath::RoundToInt((Angle2)) % 8;
	EigthDirections = static_cast<EEightCardinalDirection>(Octant);
	float Atan2Angle = FMath::Atan2(LocalVelocity.Y, LocalVelocity.X);
	int32 Dir = FMath::RoundToInt((Atan2Angle * 2 / PI) + 4) % 4;
	
	EFourCardinalDirection NewDir = static_cast<EFourCardinalDirection>(Dir);
	
	
	switch (EigthDirections)
	{
	case EEightCardinalDirection::S:
		break;
	case EEightCardinalDirection::NE:
		break;
	case EEightCardinalDirection::W:
		break;
	case EEightCardinalDirection::SE:
		break;
	case EEightCardinalDirection::N:
		break;
	case EEightCardinalDirection::SW:
		break;
	case EEightCardinalDirection::E:
		break;
	case EEightCardinalDirection::NW:
	{
		/*if(OldFourDirections == EFourCardinalDirection::W)
			NewDir = EFourCardinalDirection::N;*/
		break;
	}
	default:
		break;
	}

	if (NewDir != FourDirections)
	{
		OldFourDirections = FourDirections;
	}
	FourDirections = NewDir;
	
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 135), "OldFourDirections: " + DirToString(OldFourDirections), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 140), "FourDirections: " + DirToString(FourDirections), nullptr, FColor::Red, 0, true);

	float VelAccelDot = FVector::DotProduct(LocalVelocity, LocalAcceleration);
	int32 intDot = FMath::RoundToInt(VelAccelDot);
	OrientationDOT = VelAccelDot;
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 150), "VelAccelDot: " + FString::FormatAsNumber(intDot), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 155), "FVelAccelDot: " + FString::Printf(TEXT("%f"), VelAccelDot), nullptr, FColor::Red, 0, true);

		
	float TargetForward = FVector::DotProduct(CurrentVelocity, Forward);
	ForwardDirection = FMath::FInterpConstantTo(ForwardDirection, TargetForward, DeltaSeconds, 100.0f);

	float LateralForward = FVector::DotProduct(CurrentVelocity, Right);
	LateralDirection = FMath::FInterpConstantTo(LateralDirection, LateralForward, DeltaSeconds, 100.0f);

	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 165), "ForwardDirection: " + FString::Printf(TEXT("%f"), ForwardDirection), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 170), "LateralDirection: " + FString::Printf(TEXT("%f"), LateralDirection), nullptr, FColor::Red, 0, true);

	FString SVelocity = "V: " + FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentVelocity.Size())) + " LV: " + FString::Printf(TEXT("%f"), LocalVelocity.Size());
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 90), SVelocity, nullptr, FColor::Red, 0, true);

	FString SAcceleration = "A: " + FString::Printf(TEXT("%d"), FMath::RoundToInt(CurrentAcceleration.Size())) + " LA: " + FString::Printf(TEXT("%f"), LocalAcceleration.Size());
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 95), SAcceleration, nullptr, FColor::Red, 0, true);
	
	FVector LocalVel = Transform.InverseTransformVector(CurrentVelocity);

	OldOrient = CurrentOrient;
	FQuat VelQuat = LocalVelocity.ToOrientationQuat();
	//FRotator VelRot = VelQuat.GetAngle();
	float VelAngle = FMath::RadiansToDegrees(VelQuat.GetAngle());
	switch (FourDirections)
	{
	case EFourCardinalDirection::E:
	{
		FQuat LeftQuat = FQuat::FindBetween(Right, CurrentVelocity);
		OrientE = FRotator(LeftQuat).Yaw;
		CurrentOrient = OrientE;

		break;
	}
	case EFourCardinalDirection::N:
	{
		FQuat ForwardQuat = FQuat::FindBetween(Forward, CurrentVelocity);
		OrientN = FRotator(ForwardQuat).Yaw;
		float sign = FMath::Sign(OldOrient);
		CurrentOrient = OrientN;
		break;
	}
	case EFourCardinalDirection::W:
	{
		FQuat RightQuat = FQuat::FindBetween(Right*(-1), CurrentVelocity);
		OrientW = FRotator(RightQuat).Yaw;
		CurrentOrient = OrientW;
		break;
	}
	case EFourCardinalDirection::S:
	{
		FQuat BackQuat = FQuat::FindBetween(Forward*(-1), CurrentVelocity);
		OrientS = FRotator(BackQuat).Yaw;

		CurrentOrient = OrientW;
		break;
	}
	default:
		break;
	}
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 105), "OrientN: " + FString::FormatAsNumber(OrientN) + FString::Printf(TEXT(" DOT: %f"), FVector::DotProduct(Forward, VelocityDirection)), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 110), "OrientS: " + FString::FormatAsNumber(OrientS) + FString::Printf(TEXT(" DOT: %f"), FVector::DotProduct((-1)*Forward, VelocityDirection)), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 115), "OrientE: " + FString::FormatAsNumber(OrientE) + FString::Printf(TEXT(" DOT: %f"), FVector::DotProduct(Right, VelocityDirection)), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 120), "OrientW: " + FString::FormatAsNumber(OrientW) + FString::Printf(TEXT(" DOT: %f"), FVector::DotProduct(Right*(-1), VelocityDirection)), nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 125), "CurrentOrient: " + FString::FormatAsNumber(CurrentOrient), nullptr, FColor::Red, 0, true);

// FMath::RadiansToDegrees(FMath::Atan2(LocalVelocity.Y, LocalVelocity.X));
	float VelAngle2 = (FMath::RoundToInt(VelAngle) + 360) / 360;

	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 130), "VelAngle2: " + FString::FormatAsNumber(VelAngle2), nullptr, FColor::Red, 0, true);

	float DeltaVelocity = CurrentVelocity.Size()* DeltaSeconds;
	if (!CurrentAcceleration.IsZero())
	{
		bStopDistancePredicted = false;
	}
	if (CurrentAcceleration.Size() <= 0
		&& CurrentVelocity.Size() < CMC->MaxWalkSpeed*0.75)
	{
		bStartedLocomotion = false;
	}

	if (CurrentAcceleration.Size() > 0
		&& CurrentVelocity.Size() > 0)
	{
		bStartedLocomotion = true;
	}

	FVector FinalDestination = (AccelerationDirection * 400) + CharLocation;
	float AccelSpeed = CurrentAcceleration.Size();
	if (!bStopDistancePredicted && CurrentAcceleration.IsZero())
	{
		bStopDistancePredicted = true;
		float CurVel = CurrentVelocity.SizeSquared();
		float StopDistance = (CurVel / (4*CMC->GroundFriction *CMC->BrakingFrictionFactor * CMC->BrakingDecelerationWalking));
		FVector Forward = VelocityDirection;
		FVector StopLocation = (Forward*StopDistance) + CharLocation;
		//DrawDebugSphere(GetWorld(), StopLocation, 6, 8, FColor::Green, false, 2, 0, 2);
	}
	
	//DrawDebugSphere(GetWorld(), FinalDestination, 6, 8, FColor::Red, false, -1, 0, 2);
	float Offset = 20;
	for (float Idx = 1; Idx < 20; Idx++)
	{
		FVector BetweenDestination = (AccelerationDirection * Offset) + CharLocation;
		Offset += 20;
		//DrawDebugSphere(GetWorld(), BetweenDestination, 6, 8, FColor::Red, false, -1, 0, 2);
	}
	FString SAngle = FString("Angle: ") + FString::FormatAsNumber(Angle2);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 80), SAngle, nullptr, FColor::Red, 0, true);
	//DrawDebugString(GetWorld(), GetActorLocation() + FVector(0, 0, 60), "4: Fourtant : " + FString::FormatAsNumber(Fourtant), nullptr, FColor::Red, 0, true);
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
void AARCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	ClientPossesedBy(NewController);
	//if (AARPlayerController* PC = Cast<AARPlayerController>(Controller))
	//{
	//	WeaponInventory->InitializeInventory();
	//}
}
void AARCharacter::ClientPossesedBy_Implementation(AController* NewController)
{

}
void AARCharacter::OnRep_Controller()
{
	Super::OnRep_Controller();

}
/* IIFInventoryInterface */
void AARCharacter::OnInventoryReplicated(class UIFInventoryComponent* Inventory)
{

}
/* IIFInventoryInterface */



void AARCharacter::CalculateSpatialGrid()
{
	FVector Location = GetActorLocation();
	
	auto resFunc = [Location]()
	{
		int32 MaxH = 100;
		int32 MaxV = 20;
		float HOffset = 0;
		float VOffset = 0;
		FPostResult Res;
		for (int32 IdxH = 0; IdxH < MaxH; IdxH++)
		{
			HOffset += 30;
			VOffset = 0;
			for (int32 IdxV = 0; IdxV < MaxV; IdxV++)
			{
				VOffset += 30;
				FVector Pos = Location + FVector(HOffset, VOffset, 0);
				Res.Data.Add(Pos);
			}
		}
		return Res;
	};
	Result = Async<FPostResult>(EAsyncExecution::TaskGraph, resFunc, SpatialComplete());
}

TFunction<void()> AARCharacter::SpatialComplete()
{
	
	//DrawBox()
	auto func = [&]()
	{
		auto func2 = [&]()
		{
			for (const FVector& Pos : Result.Get().Data)
			{
				DrawDebugPoint(GetWorld(), Pos, 20, FColor::Red, true, 10);
			}
		};
		AsyncTask(ENamedThreads::GameThread, func2);
	};
	return func;
}