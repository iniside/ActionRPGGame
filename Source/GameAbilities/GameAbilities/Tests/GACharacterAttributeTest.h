// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "../IGAAbilities.h"
#include "GACharacterAttributeTest.generated.h"

UCLASS()
class GAMEABILITIES_API AGACharacterAttributeTest : public ACharacter, public IIGAAbilities
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
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual class UGAAbilitiesComponent* GetAbilityComp() override;
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override { return 0; };
};
