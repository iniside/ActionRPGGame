// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/ChildActorComponent.h"
#include "GameplayTags.h"
#include "AFEffectsComponent.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"
#include "OrionInterface.h"
#include "OrionAnimComponent.h"
#include "IFInventoryInterface.h"
#include "Weapons/ARWeaponInventoryComponent.h"

#include "OrionAnimComponent.h"

#include "ARCharacter.generated.h"
USTRUCT(BlueprintType)
struct FARCameraTransform
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
		FVector ForwardVector;
	UPROPERTY(BlueprintReadOnly)
		FVector Location;
};
struct WeaponSocket
{
	static const FName HolsteredRightWeapon;
	static const FName HolsteredLeftWeapon;

	static const FName EquipedMainWeapon;
};

UENUM(BlueprintType)
enum class AbilityInput : uint8
{
	Shoot = 0,
	Reload = 1,
	NextWeapon = 2,
	PreviousWeapon = 3,
	NextAbilitySet = 4,
	PreviousAbilitySet = 5,
	Ability01 = 6,
	Ability02 = 7,
	Ability03 = 8,
	Ability04 = 9
};

UCLASS(config=Game)
class AARCharacter : public ACharacter, public IAFAbilityInterface, public IOrionInterface, public IIFInventoryInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAFAbilityComponent* Abilities;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UAFEffectsComponent* EffectsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UOrionAnimComponent* OrionAnimComp;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UARWeaponInventoryComponent* WeaponInventory;

protected:
	UPROPERTY(EditAnywhere, Category = "Default Abilities")
		TArray<FGameplayTag> AbilitiesToGive;

	//Character parts:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Head;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Shoulders;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Arms;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Hands;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Torso;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Legs;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Feets;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* Backpack;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character Parts", meta = (AllowPrivateAccess = "true"))
		USkeletalMeshComponent* LegsCloth;


	//Weapons
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* WeaponHolsteredRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* WeaponHolsteredLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* HolsteredBackDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* WeaponHolsteredSideLeft;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
		UChildActorComponent* WeaponEquipedMain;
public:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Player Character Camera")
		FARCameraTransform CameraTransform;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test Ability")
		TSoftClassPtr<UGAAbilityBase> TestAbility01;
	UPROPERTY(Transient)
		FAFAbilitySpecHandle TestAbility01Handle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test Ability")
		TSoftClassPtr<UGAAbilityBase> TestAbility02;
	UPROPERTY(Transient)
		FAFAbilitySpecHandle TestAbility02Handle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test Ability")
		TSoftClassPtr<UGAAbilityBase> TestAbility03;
	UPROPERTY(Transient)
		FAFAbilitySpecHandle TestAbility03Handle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Test Ability")
		TSoftClassPtr<UGAAbilityBase> TestAbility04;
	UPROPERTY(Transient)
		FAFAbilitySpecHandle TestAbility04Handle;
public:
	AARCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PostInitializeComponents() override;
	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	void OnAbility01Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle);
	void OnAbility02Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle);
	void OnAbility03Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle);
	void OnAbility04Ready(FAFAbilitySpec Spec, FAFAbilitySpecHandle ServerHandle, FAFAbilitySpecHandle ClientHandle);

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/* IAFAbilityInterface- BEGIN */

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFAbilityComponent* GetAbilityComp() override;
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFEffectsComponent* GetEffectsComponent() override;
	virtual class UAFEffectsComponent* NativeGetEffectsComponent() const override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty, const FGAEffectContext& InContext) override;
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override;
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override;

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;
	/* IAFAbilityInterface- END */

	void OnCameraTransformUpdate(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);

	inline UChildActorComponent* GetHolsteredRightWeapon()
	{
		return WeaponHolsteredRight;
	}
	inline UChildActorComponent* GetHolsteredLeftWeapon()
	{
		return WeaponHolsteredLeft;
	}
	inline UChildActorComponent* GetHolsteredBackDownWeapon()
	{
		return HolsteredBackDown;
	}
	inline UChildActorComponent* GetHolsteredSideLeftWeapon()
	{
		return WeaponHolsteredSideLeft;
	}

	inline UChildActorComponent* GetEquipedMainWeapon() const
	{
		return WeaponEquipedMain;
	}

	template<typename T>
	T* GetMainWeapon() const
	{
		return Cast<T>(GetEquipedMainWeapon()->GetChildActor());
	}

	UFUNCTION(BlueprintPure, Category = "Character | Weapons")
	class AARWeaponBase* GetMainWeapon() const;

	UFUNCTION(BlueprintPure, Category = "Character | Weapons")
		USkeletalMeshComponent* GetMainWeaponMesh() const;

	UFUNCTION(BlueprintPure, Category = "Character | Weapons")
		FVector GetMainWeaponSocket(const FName& Socket) const;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION(Client, Reliable)
		void ClientPossesedBy(AController* NewController);
	void ClientPossesedBy_Implementation(AController* NewController);

	virtual void OnRep_Controller() override;
	
	/* IIFInventoryInterface */
	virtual void OnInventoryReplicated(class UIFInventoryComponent* Inventory) override;
	/* IIFInventoryInterface */



	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame")
		void CalculateSpatialGrid();

	struct FPostResult
	{
		TArray<FVector> Data;
	};
	TFuture<FPostResult> Result;
	TFunction<void()> SpatialComplete();
};

