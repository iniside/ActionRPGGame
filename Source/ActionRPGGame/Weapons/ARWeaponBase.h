// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTags.h"
#include "AMTypes.h"
#include "ARWeaponBase.generated.h"

UCLASS()
class ACTIONRPGGAME_API AARWeaponBase : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		FGameplayTag WeaponAbility;
	UPROPERTY(EditAnywhere)
		FName SocketName;
	UPROPERTY()
		APawn* POwner;

public:	
	// Sets default values for this actor's properties
	AARWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Equip(EAMGroup Group, EAMSlot Slot
		, class APawn* InPOwner
		, class UAMAbilityManagerComponent* WeaponManager);

	UFUNCTION()
	virtual void NativeOnWeaponEquiped();
	
};
