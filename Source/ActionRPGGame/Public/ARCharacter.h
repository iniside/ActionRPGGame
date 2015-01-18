// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "GSCharacter.h"

#include "GAAttributeComponent.h"
#include "GISInventoryBaseComponent.h" //need for template
#include "GESEffectComponent.h"

#include "Abilities/GSAbilitiesComponent.h"

#include "GAGlobalTypes.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "GESEffectManager.h"

#include "IGAAttributes.h"
#include "IGESEffect.h"
#include "IGESEffectManager.h"
#include "IGTSocket.h"

#include "ARCharacter.generated.h"

UCLASS(config=Game)
class AARCharacter : public AGSCharacter, public IIGAAttributes, public IIGESEffect, public IIGESEffectManager
	, public IIGTSocket
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGISInventoryBaseComponent* Inventory;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGSAbilitiesComponent* ActionBar;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGSAbilitiesComponent* StaticActionBar;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	class UGSAbilitiesComponent* AbilityBook;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attributes", meta = (AllowPrivateAccess = "true"))
	class UGAAttributeComponent* Attributes;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UGESEffectComponent* GameEffects;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Effects", meta = (AllowPrivateAccess = "true"))
	class UGESEffectManager* GameEffectManager;

public:
	/** IIGAAttributes Begin */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributesBase* GetAttributes() override;

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		virtual class UGAAttributeComponent* GetAttributeComponent() override;
	/** IIGAAttributes End */

	/** IIGESEffect Begin */
	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		virtual class UGESEffectComponent* GetEffectComponent();

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		virtual class UGESEffectManager* GetEffectManager();
	/** IIGESEffect End */

	/** IGTSocket Begin */
	virtual FVector GetSocketLocation(FName SocketNameIn) override;
	/** IGTSocket End */

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
	/*
		How to make it work in case where I would need to copy these pointer to other input buttons ?
		let's think...
	*/
	void ActivateAbility();
	template<int32 TabIndex, int32 SlotIndex>
	void InputActionBarPressed()
	{
		//let's say that only one tab in binding 1-7 can be active
		//from binding 8-12 they are always active
		//like Guild Wars 2 hotbar. Proof of concept!
		if (ActionBar->Tabs.InventoryTabs[TabIndex].bIsTabActive)
		{
			ActionBar->InputSlotPressed(TabIndex, SlotIndex);
		}
	}

	template<int32 TabIndex, int32 SlotIndex>
	void InputActionBarReleased()
	{
		if(ActionBar->Tabs.InventoryTabs[TabIndex].bIsTabActive)
		{
			ActionBar->InputSlotReleased(TabIndex, SlotIndex);
		}
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

