#pragma once
#include "../GAGlobalTypes.h"
#include "./Effects/GAGameEffect.h"
#include "../Effects/GAEffectGlobalTypes.h"

// Messaging
#include "Messaging.h"

#include "GAAttributeBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FGAGenericAttributeDelegate);

DECLARE_STATS_GROUP(TEXT("Attribute"), STATGROUP_Attribute, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("CalculateBonus"), STAT_CalculateBonus, STATGROUP_Attribute, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("CurrentBonusByTag"), STAT_CurrentBonusByTag, STATGROUP_Attribute, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("FinalBonusByTag"), STAT_FinalBonusByTag, STATGROUP_Attribute, );


struct QueuedAttributeMods
{
public:
	FGAEffectHandle Handle;
	FGAEffectMod Mod;

	void operator=(const QueuedAttributeMods& Other)
	{
		Handle = Other.Handle;
		Mod = Other.Mod;
	}
};
/*
	I probabaly should chaange attribute to use int's instead of floats. Stable, accurate and
	I can still have decimal values with them.
*/
/*
	Base data structure describing Attribute:
	1. Base Value - the base value attribute has been initialized with.
	2. Current value - current value of attribute.
	3. Bonuses value.
	4. One bonus value (if need be attribute can take care of calculating it's own bonus value).

	As for now it's made to in mind to use it as meta attribute. Meta attributes, have always
	0 value and are used to change other attributes. Like Damage is used to subtract health.
	LifeSteal is used to transfer health from target to instigator etc.

	We could extend attributes, to also support tags, so their bonus can be recalculated,
	based on on tags requiremnts from effect asking for this particular attribute.
*/
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FAFAttributeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		float BaseValue;
	UPROPERTY(EditAnywhere)
		float MinValue;
	UPROPERTY(EditAnywhere)
		float MaxValue;
	UPROPERTY(EditAnywhere)
		float CurrentValue;
	UPROPERTY(EditAnywhere)
		float BonusValue;

	UPROPERTY(EditAnywhere)
		EAFAttributeStacking Stacking;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		TSubclassOf<class UGAAttributeExtension> ExtensionClass;

	TArray<TMap<FGAEffectHandle, FGAEffectMod>> Modifiers;
	FAFAttributeBase();
	FAFAttributeBase(float BaseValueIn);
	void InitializeAttribute();
	/* You should never use those tree function to set attributes.
	Only use them for testing/debugging and setting initial values for attributes. */
	inline void SetBaseValue(float ValueIn) { BaseValue = ValueIn; }
	inline void SetMinValue(float ValueIn) { MinValue = ValueIn; }
	inline void SetMaxValue(float ValueIn) { MaxValue = ValueIn; }
	
	inline float GetFinalValue()
	{
		return FMath::Clamp<float>(BaseValue + BonusValue, MinValue, MaxValue);
	};
	inline float GetCurrentValue() { return CurrentValue; };
	void CalculateBonus();
	bool CheckIfModsMatch(const FGAEffectHandle& InHandle, const FGAEffectMod& InMod);
	bool CheckIfStronger(const FGAEffectHandle& InHandle, const FGAEffectMod& InMod);
	float Modify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);
	float DurationModify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn);
	void AddBonus(const FGAEffectMod& ModIn, const FGAEffectHandle& Handle);
	void RemoveBonus(const FGAEffectHandle& Handle, EGAAttributeMod InMod);
	EAFAttributeStacking GetStacking() const { return Stacking; }
};
template<>
struct TStructOpsTypeTraits<FAFAttributeBase> : public TStructOpsTypeTraitsBase2<FAFAttributeBase>
{
	enum
	{
		WithCopy = false
	};
};
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAModifiedAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	*	Always increment it, to make sure it will replicate.
	*/
	UPROPERTY()
		int8 ReplicationCounter;

	/**
	*	Attribute we have modified.
	*/
	UPROPERTY()
		FGAAttribute Attribute;

	/**
	*	Final value by which we modified attribute.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		float ModifiedByValue;

	/**
	*	Final tags appiled by this change.
	*/
	UPROPERTY()
		FGameplayTagContainer Tags;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
		FVector TargetLocation; //change to vector, we need only position.
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		FVector InstigatorLocation;

	UPROPERTY()
		TWeakObjectPtr<class UGAAbilitiesComponent> Causer;
};