// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "../IGAAttributes.h"
#include "GACharacterAttributeTest.generated.h"

UCLASS()
class GAMEATTRIBUTES_API AGACharacterAttributeTest : public ACharacter, public IIGAAttributes
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UGAAttributeComponent* Attributes;
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
	virtual class UGAAttributeComponent* GetAttributeComponent() override;
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override { return 0; };
};
