// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/ShapeComponent.h"
#include "GameplayTags.h"
#include "AMTypes.h"
#include "AMAbilityManagerComponent.h"
#include "AFDAbilityGiveTrigger.generated.h"

USTRUCT(BlueprintType)
struct FAFDAbilityConfig
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		EAMGroup Group;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		EAMSlot Slot;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		TSoftClassPtr<UGAAbilityBase> AbilityTag;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		USceneComponent* RootComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBillboardComponent* Icon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBoxComponent* Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UShapeComponent* Trigger2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
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
		void OnAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput);

	UFUNCTION()
		void OnAbilityInputReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput);

	
};
