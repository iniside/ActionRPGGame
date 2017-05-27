// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GACueActor.generated.h"

UCLASS()
class ABILITYFRAMEWORK_API AGACueActor : public AActor
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	class UGAAbilityBase* OwningAbility;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Root")
		USceneComponent* DefaultRoot;

public:	
	// Sets default values for this actor's properties
	AGACueActor(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
	
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnDeactivated();
	/*
		Mirros what is inside Object version. 
	*/
	/**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityActivationStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityPeriod();

	//from ability anim notifies:

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityStartNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityEndNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateEnd();
};
