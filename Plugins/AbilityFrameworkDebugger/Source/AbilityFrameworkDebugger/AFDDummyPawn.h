// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/ArrowComponent.h"
#include "Components/CapsuleComponent.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"
#include "AFDDummyPawn.generated.h"

UCLASS()
class ABILITYFRAMEWORKDEBUGGER_API AAFDDummyPawn : public APawn, public IAFAbilityInterface
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCapsuleComponent* Collision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UAFAbilityComponent* Abilities;
public:
	// Sets default values for this pawn's properties
	AAFDDummyPawn(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/* AFAbilityInterface Implementation START */
	virtual FVector GetSocketLocation(FName SocketNameIn) override { return FVector::ZeroVector; };

	virtual class UGAAttributesBase* GetAttributes() override;

	virtual class UAFAbilityComponent* GetAbilityComp() override;

	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;

	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn,
		struct FGAEffectProperty& InProperty) override;

	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override;

	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override;

	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;

	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn
		, FGAEffectProperty& InProperty
		, FGAEffectContext& InContext) override;

	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override {};
	/* AFAbilityInterface Implementation END */
};
