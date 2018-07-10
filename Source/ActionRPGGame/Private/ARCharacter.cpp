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
#include "ARAbilityBase.h"

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

	OrionAnimComp = CreateDefaultSubobject<UOrionAnimComponent>(TEXT("OrionAnimComp"));

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->SetupAttachment(GetMesh());
	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("headSocket"));
	
	CameraBoom->TargetArmLength = 285; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->SocketOffset = FVector(0, 35, 65);
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
	
	Shoulders = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Shoulders"));
	Shoulders->SetupAttachment(GetMesh());
	

	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(GetMesh());
	

	Hands = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Hands"));
	Hands->SetupAttachment(GetMesh());
	

	Torso = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Torso"));
	Torso->SetupAttachment(GetMesh());
	

	Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs"));
	Legs->SetupAttachment(GetMesh());
	

	Feets = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Feets"));
	Feets->SetupAttachment(GetMesh());
	

	Backpack = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Backpack"));
	Backpack->SetupAttachment(GetMesh());
	

	LegsCloth = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsCloth"));
	LegsCloth->SetupAttachment(GetMesh());
	

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
void AARCharacter::OnConstruction(const FTransform& Transform)
{
	Head->SetMasterPoseComponent(GetMesh());
	Head->UpdateMasterBoneMap();

	Shoulders->SetMasterPoseComponent(GetMesh());
	Shoulders->UpdateMasterBoneMap();

	Arms->SetMasterPoseComponent(GetMesh());
	Arms->UpdateMasterBoneMap();

	Hands->SetMasterPoseComponent(GetMesh());
	Hands->UpdateMasterBoneMap();

	Torso->SetMasterPoseComponent(GetMesh());
	Torso->UpdateMasterBoneMap();

	Legs->SetMasterPoseComponent(GetMesh());
	Legs->UpdateMasterBoneMap();

	Feets->SetMasterPoseComponent(GetMesh());
	Feets->UpdateMasterBoneMap();

	Backpack->SetMasterPoseComponent(GetMesh());
	Backpack->UpdateMasterBoneMap();
}
void AARCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}
void AARCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!TestAbility01.IsNull())
	{
		TestAbility01Handle = FAFAbilitySpecHandle::GenerateHandle();
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARCharacter::OnAbility01Ready);
		Abilities->AddOnAbilityReadyDelegate(TestAbility01Handle, del1);

		Abilities->NativeAddAbility(TestAbility01, TestAbility01Handle);
	}

	if (!TestAbility02.IsNull())
	{
		TestAbility02Handle = FAFAbilitySpecHandle::GenerateHandle();
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARCharacter::OnAbility02Ready);
		Abilities->AddOnAbilityReadyDelegate(TestAbility02Handle, del1);

		Abilities->NativeAddAbility(TestAbility02, TestAbility02Handle);
	}

	if (!TestAbility03.IsNull())
	{
		TestAbility03Handle = FAFAbilitySpecHandle::GenerateHandle();
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARCharacter::OnAbility03Ready);
		Abilities->AddOnAbilityReadyDelegate(TestAbility03Handle, del1);

		Abilities->NativeAddAbility(TestAbility03, TestAbility03Handle);
	}

	if (!TestAbility04.IsNull())
	{
		TestAbility04Handle = FAFAbilitySpecHandle::GenerateHandle();
		FAFOnAbilityReady del1 = FAFOnAbilityReady::CreateUObject(this, &AARCharacter::OnAbility04Ready);
		Abilities->AddOnAbilityReadyDelegate(TestAbility04Handle, del1);

		Abilities->NativeAddAbility(TestAbility04, TestAbility04Handle);
	}

	//LegsCloth->SetMasterPoseComponent(GetMesh());
	WeaponInventory->SetIsReplicated(true);
	WeaponInventory->InitializeWeapons(this);
}

void AARCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AARCharacter::OnAbility01Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	TestAbility01Handle = ServerHandle;

	TArray<uint8> InputIds;
	InputIds.Add(static_cast<uint8>(AbilityInput::Ability01));
	Abilities->BindAbilityToInputIDs(ServerHandle, InputIds);
}
void AARCharacter::OnAbility02Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	TestAbility02Handle = ServerHandle;

	TArray<uint8> InputIds;
	InputIds.Add(static_cast<uint8>(AbilityInput::Ability02));
	Abilities->BindAbilityToInputIDs(ServerHandle, InputIds);
}
void AARCharacter::OnAbility03Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	TestAbility03Handle = ServerHandle;

	TArray<uint8> InputIds;
	InputIds.Add(static_cast<uint8>(AbilityInput::Ability03));
	Abilities->BindAbilityToInputIDs(ServerHandle, InputIds);
}
void AARCharacter::OnAbility04Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle)
{
	TestAbility04Handle = ServerHandle;

	TArray<uint8> InputIds;
	InputIds.Add(static_cast<uint8>(AbilityInput::Ability04));
	Abilities->BindAbilityToInputIDs(ServerHandle, InputIds);
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

	Abilities->BindInputs(PlayerInputComponent, "AbilityInput");
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
	struct FGAEffectProperty& InProperty, const FGAEffectContext& InContext)
{
	Abilities->ModifyAttribute(ModIn, HandleIn, InProperty, InContext);
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