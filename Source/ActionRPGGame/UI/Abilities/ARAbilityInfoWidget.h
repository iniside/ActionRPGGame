// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTags.h"
#include "Abilities/GAAbilityBase.h"
#include "ARAbilityInfoWidget.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UARAbilityInfoWidget : public UUserWidget
{
	GENERATED_BODY()
protected:
	UPROPERTY()
		FGameplayTag AbilityTag;
	UPROPERTY()
		class UGAAbilityBase* Ability;
	UPROPERTY(BlueprintReadOnly)
		class UARUIAbilityManagerComponent* OwningComponent;

public:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	virtual void InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent);
	virtual void NativeAddAbility(const FGameplayTag& InAbilityTag);
	virtual void NativeOnAbilityReady(const FGameplayTag& InAbilityTag);
	UFUNCTION(BlueprintCallable, Category = "ActionRPGGame|UI|Abilities")
		void AddAbility(const FGameplayTag& InAbilityTag);
	UFUNCTION(BlueprintCallable, DisplayName = "Initialize Ability Widget", Category = "ActionRPGGame|UI|Abilities")
		void BP_InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent);
	UFUNCTION()
		void OnAbilityReady(const FGameplayTag& InAbilityTag);
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "On Ability Ready", Category = "ActionRPGGame|UI|Abilities")
		void BP_OnAbilityReady(const FGameplayTag& OutAbilityTag);



};
