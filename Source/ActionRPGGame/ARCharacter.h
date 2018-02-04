// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTags.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"
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

UCLASS(config=Game)
class AARCharacter : public ACharacter, public IAFAbilityInterface
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

	
	UPROPERTY(EditAnywhere, Category = "Default Abilities")
		TArray<FGameplayTag> AbilitiesToGive;
public:
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Player Character Camera")
		FARCameraTransform CameraTransform;

public:
	AARCharacter();

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
	virtual void PossessedBy(AController* NewController) override;
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/* IAFAbilityInterface- BEGIN */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UGAAttributesBase* GetAttributes() override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual class UAFAbilityComponent* GetAbilityComp() override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Attributes")
		virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty) override;
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override;
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override;

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;

	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, FGAEffectContext& InContext) override;
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override;

	/* IAFAbilityInterface- END */

	UFUNCTION(NetMulticast, Unreliable)
		void Multicast_CameraTransform(FARCameraTransform InCameraTransform);
	void Multicast_CameraTransform_Implementation(FARCameraTransform InCameraTransform);

	void OnCameraTransformUpdate(USceneComponent* UpdatedComponent, EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport);
};

