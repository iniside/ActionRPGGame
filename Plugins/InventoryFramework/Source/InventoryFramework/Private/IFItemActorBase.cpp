// Fill out your copyright notice in the Description page of Project Settings.

#include "IFItemActorBase.h"


// Sets default values
AIFItemActorBase::AIFItemActorBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

// Called when the game starts or when spawned
void AIFItemActorBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIFItemActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<TSoftClassPtr<class UIFItemBase>> AIFItemActorBase::GetAllItems()
{
	Destroy();
	return Items;
}