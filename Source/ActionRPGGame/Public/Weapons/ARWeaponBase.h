// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ARWeaponBase.generated.h"

UCLASS()
class ACTIONRPGGAME_API AARWeaponBase : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* Mesh;
public:	
	// Sets default values for this actor's properties
	AARWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	class USkeletalMeshComponent* GetMesh()
	{
		return Mesh;
	}
};
