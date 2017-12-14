// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "ARAbilityBase.h"
#include "ARAvailableAbilities.h"
#include "ARUIAbilityManagerComponent.generated.h"


USTRUCT(BlueprintType)
struct FARAbilityInputBinding
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FGameplayTag> InputBinding;

	inline FGameplayTag& operator[](int32 InIndex)
	{
		return InputBinding[InIndex];
	}

	inline const FGameplayTag& operator[](int32 InIndex) const
	{
		return InputBinding[InIndex];
	}
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAROnAbilitySetChanged, int32, AbilitySet);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIAbilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Config")
		TSubclassOf<UARAvailableAbilities> Abilities;
	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UUserWidget> AbilitySetConfigClass;
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* AbilitySetConfigWidget;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityInfoWidget> AbilityWidgetClass;
	UPROPERTY(BlueprintReadOnly)
		UARAbilityInfoWidget* AbilityWidget;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARWeaponInfoWidget> WeaponWidgetClass;
	UPROPERTY(BlueprintReadOnly)
		UARWeaponInfoWidget* WeaponWidget;

	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UUserWidget> WeaponCrosshairWidgetClass;
	UPROPERTY(BlueprintReadOnly)
		UUserWidget* WeaponCrosshairWidget;

	UPROPERTY(EditAnywhere, Category = "Input Config")
		TArray<FGameplayTag> AbilityInputs;

	int32 ActiveSet;
	UPROPERTY(EditAnywhere, Category = "Input Config")
		TArray<FARAbilityInputBinding> InputBindingsSet;
	
	TArray<TArray<FGameplayTag>> AbilityTagsSet;
	TArray<TArray<TWeakObjectPtr<class UARAbilityBase>>> AbilitySet;
public:
	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityWidget> DragVisualClass;

	UPROPERTY(BlueprintAssignable)
		FAROnAbilitySetChanged OnAbilitySetChanged;
public:	
	// Sets default values for this component's properties
	UARUIAbilityManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UARAbilityBase* GetAbility(int32 SetIndex, int32 AbilityIndex);
	void SetAbility(int32 SetIndex, int32 AbilityIndex, UARAbilityBase* InAbility);
	
	FGameplayTag GetAbilityTag(int32 SetIndex, int32 AbilityIndex);
	void SetAbilityTag(int32 SetIndex, int32 AbilityIndex, FGameplayTag InAbilityTag);

	FGameplayTag GetInputTag(int32 SetIndex, int32 AbilityIndex);
	void SetInputTag(int32 SetIndex, int32 AbilityIndex, FGameplayTag InAbilityTag);

	void NativeEquipAbility(const FGameplayTag& InAbilityTag, int32 InAbilitySet
		, int32 AbilityIndex);
	UFUNCTION()
		void OnAbilityReady(FGameplayTag InAbilityTag, FGameplayTag InAbilityInput,
			int32 InAbilitySet, int32 InAbilityIndex);

	void NativeOnAbilityReady(const FGameplayTag& InAbilityTag, const FGameplayTag InAbilityInput,
		int32 InAbilitySet, int32 InAbilityIndex);

	//only used in Client-Server mode. Abilities are additionaly binded on server
	//so we need to wait for server response to make sure that ability is bound on server.
	UFUNCTION()
		void OnAbilityInputReady(FGameplayTag InAbilityTag, FGameplayTag InAbilityInput,
			int32 InAbilitySet, int32 InAbilityIndex);

	void SwitchSet();
	UFUNCTION()
	void FinishedLoadinFiles();
};
