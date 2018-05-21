// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Attributes/GAAttributesBase.h"
#include "ARCharacterAttributes.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARCharacterAttributes : public UGAAttributesBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, ReplicatedUsing=OnRep_Health, Category = "Base")
		FAFAttributeBase Health;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Shield;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Armor;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Energy;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Stamina;

	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase Ammo;
	UPROPERTY(EditAnywhere, Replicated, Category = "Base")
		FAFAttributeBase MachineGunAmmo;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase ShotgunAmmo;
	UPROPERTY(EditAnywhere, Category = "Base")
		FAFAttributeBase ArrowAmmo;
public:

	UARCharacterAttributes(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void OnRep_Health();


};
