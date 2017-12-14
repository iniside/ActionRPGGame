// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "GameplayTags.h"
#include "AMTypes.h"
#include "AMAbilityManagerComponent.h"
#include "AFDAbilityGiveTrigger.generated.h"

USTRUCT()
struct FAFDAbilityConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMGroup Group;
	UPROPERTY(EditAnywhere, Category = "Config")
		EAMSlot Slot;
	UPROPERTY(EditAnywhere, Category = "Config")
		FGameplayTag AbilityTag;
	UPROPERTY(EditAnywhere, Category = "Config")
		TArray<FGameplayTag> InputTag;
};

static const FName RootCompName = TEXT("RootComp");
static const FName TriggerName = TEXT("Trigger");
static const FName IconName = TEXT("Icon");
UCLASS()
class ABILITYFRAMEWORKDEBUGGER_API AAFDAbilityGiveTrigger : public AActor
{
	GENERATED_BODY()
protected:
	UPROPERTY(BlueprintReadOnly)
		USceneComponent* RootComp;

	UPROPERTY(BlueprintReadOnly)
		UBillboardComponent* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UBoxComponent* Trigger;


	UPROPERTY(EditAnywhere, Category = "Config")
		FAFDAbilityConfig AbilityConfig;

	UPROPERTY()
		class UAFAbilityComponent* CurrentComponent;
	UPROPERTY()
		class UAMAbilityManagerComponent* AbilityManager;
	
public:	
	// Sets default values for this actor's properties
	AAFDAbilityGiveTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
protected:
	UFUNCTION()
		void BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	/** Delegate for notification of end of overlap with a specific component */
	UFUNCTION()
		void EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void OnAbilityReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput);

	UFUNCTION()
		void OnAbilityInputReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput);

	
};
