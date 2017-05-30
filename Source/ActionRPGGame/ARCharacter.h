// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"

#include "GSCharacter.h"

#include "GAAbilitiesComponent.h"
#include "GISInventoryBaseComponent.h" //need for template

#include "Abilities/GSAbilitiesComponent.h"

#include "GAGlobalTypes.h"

#include "GISGlobalTypes.h"
#include "GISItemData.h"

#include "IGAAbilities.h"
#include "IGIPawn.h"
#include "Abilities/GSAbilitiesComponent.h"

#include "ARCharacter.generated.h"

UCLASS(config = Game)
class AARCharacter : public AGSCharacter, public IIGAAbilities,
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
    class UGAAbilitiesComponent* Attributes;

    UPROPERTY(BlueprintReadOnly, Category = "Base")
    class AARPlayerController* ARPController;
public:
    /** IIGAAbilities Begin */
    UFUNCTION(BlueprintCallable, Category = "Game Attributes")
        virtual class UGAAttributesBase* GetAttributes() override;

    UFUNCTION(BlueprintCallable, Category = "Game Attributes")
        virtual class UGAAbilitiesComponent* GetAbilityComp() override;

    UFUNCTION(BlueprintCallable, Category = "Game Attributes")
        virtual float GetAttributeValue(FGAAttribute AttributeIn) const;

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);// override { DefaultAttributes->ModifyAttribute(ModIn, HandleIn); };
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn);// override { return DefaultAttributes->GetAttribute(AttributeIn); };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod);// override { DefaultAttributes->RemoveBonus(AttributeIn, HandleIn); };
	virtual FGAEffectHandle ApplyEffectToTarget(const FGAEffect& EffectIn, const FGAEffectHandle& HandleIn,
		FGAEffectProperty& InProperty) override;// { return ApplyEffectToTarget(EffectIn, HandleIn); };
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const;// override { return 0; };

    /** IIGAAbilities End */

    /** IIGIPawn */
    virtual APawn* GetGamePawn() override { return this; };
    virtual ACharacter* GetGameCharacter() override { return this; }
    virtual AController* GetGameController() override { return GetController(); }
    virtual APlayerController* GetGamePlayerController() override;
	virtual UCameraComponent* GetPawnCamera() override { return FollowCamera; };
    /* IIGIPawn **/

	UFUNCTION(BlueprintPure, Category="Animation")
		int32 GetMoveDirection() const
	{
		return MoveDirection;
	}
	UPROPERTY()
		int32 MoveDirection;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		int32 EightDirection;

	UPROPERTY()
		bool bIsPivotChanging;
	UPROPERTY()
		float MovementDot;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		float Angle;
	//float CurrentForward;
	//float OldForward;
	UPROPERTY()
		FVector AccelerationLocalDir;
	UPROPERTY()
		FVector AccelerationWorldDir;
	UPROPERTY()
		FVector VelocityLocalDir;
	UPROPERTY()
		FVector VelocityWorldDir;
	UPROPERTY()
		FRotator AccelerationRotator;
	UPROPERTY()
		FRotator VelocityRotator;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		FVector DirectionX;
	UPROPERTY(BlueprintReadOnly, Category = "Animation")
		FVector DirectionY;

	UFUNCTION(BlueprintPure, Category = "Animation")
		bool GetIsPivotChanging() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
		float GetMovementDot() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
		FVector GetAccelerationLocalDir() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
		FVector GetAccelerationWorldDir() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
		FVector GetVelocityLocalDir() const;

	UFUNCTION(BlueprintPure, Category = "Animation")
		FVector GetVelocityWorldDir() const;
	UFUNCTION(BlueprintPure, Category = "Animation")
		FRotator GetAccelerationRotator() const;
	UFUNCTION(BlueprintPure, Category = "Animation")
		FRotator GetVelocityRotator() const;

    virtual void OnRep_Controller() override;
    virtual void PossessedBy(AController* NewController) override;
public:
    AARCharacter(const FObjectInitializer& ObjectInitializer);

    virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
    /** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseTurnRate;

    /** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        float BaseLookUpRate;

    UPROPERTY(BlueprintReadOnly, Category = "Movement")
        bool bIsStrafingLeft;
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
        bool bIsStrafingRight;
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
        bool bIsMovingForward;
    UPROPERTY(BlueprintReadOnly, Category = "Movement")
        bool bIsMovingBackward;
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
       // Abilities->InputPressed(SetIndex, SlotIndex);
    }

    template<int32 SetIndex, int32 SlotIndex>
    void InputActionBarReleased()
    {
       // Abilities->InputReleased(SetIndex, SlotIndex);
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
    virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;
    // End of APawn interface

public:
    /** Returns CameraBoom subobject **/
    FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    /** Returns FollowCamera subobject **/
    FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

