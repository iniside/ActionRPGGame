// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GAEffectInstanced.h"
#include "GASEffectActor.generated.h"

UCLASS()
class GAMEABILITIES_API AGASEffectActor : public AActor
{
	GENERATED_BODY()
public:
	/*
		Effect actor is just in world container for instanced effect.
		EffectInstanced actually implements all game logic behind it.
	*/
	UPROPERTY()
	class UGAEffectInstanced* EffectInstanced;
public:	
	// Sets default values for this actor's properties
	AGASEffectActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
	
};
