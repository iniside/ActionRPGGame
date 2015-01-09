// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "GSCharacter.generated.h"

UCLASS(config=Game)
class GAMESYSTEM_API AGSCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment")
	class UGSEquipmentComponent* Equipment;
public:
	AGSCharacter(const FObjectInitializer& ObjectInitializer);

	void BeginPlay() override;

	void SwapSkeletalMesh(USkeletalMesh* SkeletalMesh);
};

