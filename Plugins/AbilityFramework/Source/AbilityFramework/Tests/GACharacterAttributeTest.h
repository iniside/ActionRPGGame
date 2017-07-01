// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "../IGAAbilities.h"
#include "GACharacterAttributeTest.generated.h"

UCLASS()
class ABILITYFRAMEWORK_API AGACharacterAttributeTest : public ACharacter, public IIGAAbilities
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UGAAbilitiesComponent* Attributes;
public:
	// Sets default values for this character's properties
	AGACharacterAttributeTest();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InInputComponent) override;

	
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual class UGAAbilitiesComponent* GetAbilityComp() override;
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override { return 0; };
	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);// override { DefaultAttributes->ModifyAttribute(ModIn, HandleIn); };
	
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn);// override { return DefaultAttributes->GetAttribute(AttributeIn); };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override;
	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty, FGAEffectContext& InContext) override;// { return ApplyEffectToTarget(EffectIn, HandleIn); };
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const;// override { return 0; };
};
