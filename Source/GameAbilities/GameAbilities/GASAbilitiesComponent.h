// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GASAbilitiesComponent.generated.h"

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASOwnedAbilities
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TSubclassOf<class UGASAbility> AbilityClass;
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbilitySlot
{
	GENERATED_USTRUCT_BODY()
public:
	int32 SlotIndex;

	UPROPERTY()
	class UGASAbility* ActiveAbility;

	void OnAbilityAdded()
	{
	};

	FGASActiveAbilitySlot()
	{
		ActiveAbility = nullptr;
		SlotIndex = INDEX_NONE;
	};
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASActiveAbilitiesConfig
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Config")
		int32 MaxAbilities;
};

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEABILITIES_API UGASAbilitiesComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGASAbilitiesComponent(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(ReplicatedUsing = OnRep_InstancedAbilities)
		TArray<FGASActiveAbilitySlot> InstancedAbilities;
	UFUNCTION()
		void OnRep_InstancedAbilities();

	UPROPERTY(EditAnywhere, Category = "Config")
		FGASActiveAbilitiesConfig InstancedAbilitiesConfig;

	/*
		True if player is currently casting/channeling/activating(?)
		any ability.
	*/
	bool bIsAnyAbilityActive;
private:
	class IIGIPawn* PawnInterface;
public:
	/*
		Checks on client and server, if we can activate ability. Called from InputPressed
	*/
	virtual bool CanActivateAbility();

	virtual void InitializeComponent() override;

	inline class UGASAbility* GetGASAbility(int32 IndexIn)
	{
		if (InstancedAbilities.IsValidIndex(IndexIn))
			return InstancedAbilities[IndexIn].ActiveAbility;
		return nullptr;
	}

	void InputPressed(int32 AbilityId);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputPressed(int32 AbilityId);
	virtual void ServerInputPressed_Implementation(int32 AbilityId);
	virtual bool ServerInputPressed_Validate(int32 AbilityId);

	void InputReleased(int32 AbilityId);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerInputReleased(int32 AbilityId);
	virtual void ServerInputReleased_Implementation(int32 AbilityId);
	virtual bool ServerInputReleased_Validate(int32 AbilityId);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_AddAbility(TSubclassOf<class UGASAbility> AbilityClass);
	/*
		Should be called on server.
		Adds new ability to ActiveAbilities;
	*/
	int32 AddAbilityToActiveList(TSubclassOf<class UGASAbility> AbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_GiveAbility(TSubclassOf<class UGASAbility> AbilityClass);



	void GiveAbility(TSubclassOf<class UGASAbility> AbilityClass);

	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
protected:
	void InitializeInstancedAbilities();
};



