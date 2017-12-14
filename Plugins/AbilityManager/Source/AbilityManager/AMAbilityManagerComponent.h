// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AMTypes.h"
#include "GameplayTags.h"
#include "Abilities/GAAbilityBase.h"
#include "AMAbilityManagerComponent.generated.h"

/*
- Group
- Set
- Ability
-Inputs (multiple);

*/

//all the inputs assigned to SINGLE ability;
USTRUCT()
struct FAMAbilityInputs
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FGameplayTag> Inputs;
};

USTRUCT()
struct FAMAbilityInputSlot
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		FAMAbilityInputs Inputs;

	TArray<FGameplayTag> operator()()
	{
		return Inputs.Inputs;
	}
};


USTRUCT()
struct FAMAbilityInputGroup
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FAMAbilityInputSlot> Slots;
};


USTRUCT()
struct FAMAbilityInputContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FAMAbilityInputGroup> Groups;


	TArray<FGameplayTag> GetInputs(EAMGroup InGroup, EAMSlot InSlot)
	{
		return Groups[AMEnumToInt<EAMGroup>(InGroup)].Slots[AMEnumToInt<EAMSlot>(InSlot)]();
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYMANAGER_API UAMAbilityManagerComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere)
		uint8 MaxGroups;

	UPROPERTY(EditAnywhere)
		FAMAbilityInputContainer InputSetup;

	UPROPERTY(EditAnywhere, Category = "Input Config")
		TArray<FGameplayTag> InputsToBind;

	EAMGroup ActiveGroup;
	//UPROPERTY(EditAnywhere, Category = "Input Config")
	//	TArray<FARAbilityInputBinding> InputBindingsSet;

	TArray<TArray<FGameplayTag>> AbilityTagsSet;
	TArray<TArray<TWeakObjectPtr<class UGAAbilityBase>>> AbilitySet;
public:	
	// Sets default values for this component's properties
	UAMAbilityManagerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void InitializeComponent() override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void BindInputs();
	UGAAbilityBase* GetAbility(EAMGroup InGroup, EAMSlot InSlot);
	void SetAbility(EAMGroup InGroup, EAMSlot InSlot, UGAAbilityBase* InAbility);

	FGameplayTag GetAbilityTag(EAMGroup InGroup, EAMSlot InSlot);
	void SetAbilityTag(EAMGroup InGroup, EAMSlot InSlot, FGameplayTag InAbilityTag);
		
	TArray<FGameplayTag> GetInputTag(EAMGroup InGroup, EAMSlot InSlot);
	void SetInputTag(EAMGroup InGroup, EAMSlot InSlot, TArray<FGameplayTag> InAbilityTag);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Equip Ability"), Category = "Ability Manager")
		void BP_EquipAbility(const FGameplayTag& InAbilityTag, EAMGroup InGroup, EAMSlot InSlot);

	void NativeEquipAbility(const FGameplayTag& InAbilityTag, EAMGroup InGroup, EAMSlot InSlot);
	UFUNCTION()
		void OnAbilityReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput,
			EAMGroup InGroup, EAMSlot InSlot);

	UFUNCTION()
		void OnAbilityInputReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput,
			EAMGroup InGroup, EAMSlot InSlot);
	
	UFUNCTION(BlueprintCallable)
		void NextGroup();
	UFUNCTION(BlueprintCallable)
		void PreviousGroup();

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNextGroup(int32 WeaponIndex);
	void ServerNextGroup_Implementation(int32 WeaponIndex);
	bool ServerNextGroup_Validate(int32 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientNextGroup(int32 WeaponIndex);
	void ClientNextGroup_Implementation(int32 WeaponIndex);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerPreviousGroup(int32 WeaponIndex);
	void ServerPreviousGroup_Implementation(int32 WeaponIndex);
	bool ServerPreviousGroup_Validate(int32 WeaponIndex);
	UFUNCTION(Client, Reliable)
		void ClientPreviousGroup(int32 WeaponIndex);
	void ClientPreviousGroup_Implementation(int32 WeaponIndex);


	UFUNCTION(BlueprintCallable)
		void SelectGroup(EAMGroup InGroup);
};
