// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "OrionTypes.h"

#include "OrionAnimComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ORIONANIMATION_API UOrionAnimComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	EOrionCardinalDirection CardinalDirection;

	UPROPERTY()
		class ACharacter* CharacterOwner;

public:	
	// Sets default values for this component's properties
	UOrionAnimComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeAnim(ACharacter* InCharacter);

	void CalculateCardinalDirection();
};
