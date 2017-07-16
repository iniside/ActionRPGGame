// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ARAbilityData.h"
#include "GameplayTags.h"
#include "ARAbilityBase.h"
#include "ARUIAbilityManagerComponent.generated.h"

USTRUCT()
struct FARAbilityEquipInfo
{
	GENERATED_BODY()
public:
	int32 AbilitySetIndex;
	int32 AbilityIndex;
	FGameplayTag InputBinding;

	FARAbilityEquipInfo()
	{};
	FARAbilityEquipInfo(int32 InAbilitySetIndex
		, int32 InAbilityIndex
		, FGameplayTag InInputBinding
	)
		: AbilitySetIndex(InAbilitySetIndex),
		AbilityIndex(InAbilityIndex),
		InputBinding(InInputBinding)
	{};
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAROnAbilitySetChanged, int32, AbilitySet);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPGGAME_API UARUIAbilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
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

	TWeakObjectPtr<class UGAAbilityBase> ActiveWeapon;
	TWeakObjectPtr<class UGAAbilityBase> WeaponTwo;
	TWeakObjectPtr<class UGAAbilityBase> WeaponThree;
	TWeakObjectPtr<class UGAAbilityBase> WeaponFour;

	int32 ActiveSet;
	TArray<TArray<FGameplayTag>> InputBindingsSet;
	TArray<TArray<FGameplayTag>> AbilityTagsSet;
	TArray<TArray<TWeakObjectPtr<class UARAbilityBase>>> AbilitySet;
	TMap<FGameplayTag, FARAbilityEquipInfo> AwatingAbilityConfimation;
public:
	UPROPERTY(EditAnywhere, Category = "Widget Config")
		TSubclassOf<class UARAbilityWidget> DragVisualClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UARAbilityData* AbilityData;
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

	void NativeEquipAbility(const FGameplayTag& InAbilityTag, int32 AbilitySet
		, int32 AbilityIndex, const FGameplayTag& InputBinding);
	UFUNCTION()
		void OnAbilityReady(const FGameplayTag& InAbilityTag);

	void NativeOnAbilityReady(const FGameplayTag& InAbilityTag);

	void SwitchSet();
	UFUNCTION()
	void FinishedLoadinFiles();
};
