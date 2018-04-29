// Fill out your copyright notice in the Description page of Project Settings.

#include "ARWeaponBase.h"

#include "Components/SkeletalMeshComponent.h"
// Sets default values
AARWeaponBase::AARWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);
}

// Called when the game starts or when spawned
void AARWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AARWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

