// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"

#include "Abilities/GAAbilityBase.h"
#include "AMAbilityManagerComponent.h"
#include "DWTypes.h"


#include "ARAbilityManagerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAROnAbilitySetChanged, EAMGroup, Group);

USTRUCT(BlueprintType)
struct FARAbilityItem
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TSoftClassPtr<UGAAbilityBase> Ability;
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARAbilityManagerComponent : public UAMAbilityManagerComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		TArray<FARAbilityItem> AvailableAbilities;
public:
	UPROPERTY(BlueprintAssignable)
		FAROnAbilitySetChanged OnAbilitySetChanged;
public:	
	// Sets default values for this component's properties
	UARAbilityManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShowHideAbilityManager();
	
	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|Ability Manager")
		void SetCurrentSet(int32 SetIndex);

	virtual void OnGroupSelectionConfirmed(EAMGroup ValidGroup, bool bPredictionSuccess) override;

	void OnInputReady(TSoftClassPtr<UGAAbilityBase> WeaponAbilityTag, TArray<FGameplayTag> InInputTags);
};
