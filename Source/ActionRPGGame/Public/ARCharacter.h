// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "GSCharacter.h"

#include "GAAttributeComponent.h"
#include "GISInventoryBaseComponent.h" //need for template

#include "Abilities/GSAbilitiesComponent.h"

#include "GAGlobalTypes.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "IGAAttributes.h"
#include "IGIPawn.h"
#include "Abilities/GSAbilitiesComponent.h"

#include "ARCharacter.generated.h"

UCLASS(config=Game)
class AARCharacter : public AGSCharacter, public IIGAAttributes,
	public IIGIPawn
{
	GENERATED_BODY()
protected:
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	class UGAAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = "Base")
	class AARPlayerController* ARPController;
public:
	/** IIGAAttributes Begin */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributeComponent* GetAttributeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const;
	/** IIGAAttributes End */

	/** IIGIPawn */
	virtual APawn* GetGamePawn() override { return this; };
	virtual ACharacter* GetGameCharacter() override { return this; }
	virtual AController* GetGameController() override { return GetController(); }
	virtual APlayerController* GetGamePlayerController() override;
	/* IIGIPawn **/


	virtual void OnRep_Controller() override;
	virtual void PossessedBy(AController* NewController) override;
public:
	AARCharacter(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
protected:
	/*
		Template. Becuase creating something like 60 functions would be sooo cool!
	*/

	void InputGetNextLeftWeapon();
	void InputGetNextRightWeapon();

	void InputUseLeftWeaponPressed();
	void InputUseLeftWeaponReleased();

	void InputUseRightWeaponPressed();
	void InputUseRightWeaponReleased();

	//reload both weapnons if both are equiped.
	void InputReloadWeapon();
	
	
	void InputAbilityPressed();
	void InputAbilityReleased();
	/*
		How to make it work in case where I would need to copy these pointer to other input buttons ?
		let's think...
	*/
	void ActivateAbility();
	template<int32 SetIndex, int32 SlotIndex>
	void InputActionBarPressed()
	{
		Abilities->InputPressed(SetIndex, SlotIndex);
	}

	template<int32 SetIndex, int32 SlotIndex>
	void InputActionBarReleased()
	{
		Abilities->InputReleased(SetIndex, SlotIndex);
	}

	void ShowHideEditableHotbars();

	/*
		This will cycle trough action bars.
	*/
	void InputSwapActionBars();

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

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

	void ShowHideInventory();

	void ShowHideAbilityBook();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character Death")
		void OnCharacterDied();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character Death")
		void OnPostControllerRep(class AARPlayerController* ControllerOut);
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

