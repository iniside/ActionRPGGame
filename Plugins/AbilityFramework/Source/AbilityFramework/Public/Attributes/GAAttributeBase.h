#pragma once
#include "../GAGlobalTypes.h"
#include "./Effects/GAGameEffect.h"
#include "../Effects/GAEffectGlobalTypes.h"

// Messaging
//#include "Messaging.h"

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

//wrapper for multi-precision attribute.
//works like any numeric type, but depending on preprocessor flag it can int16,in32, float or double.

typedef int16 AttributeReal;

struct FNumericAttribute
{
	AttributeReal Value;
};

class UAFAbilityComponent;
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

	UPROPERTY(NotReplicated)
		class UAFAbilityComponent* AbilityComp;
	UPROPERTY(NotReplicated)
		FGAAttribute SelfName;

	UPROPERTY(EditAnywhere, SaveGame)
		float BaseValue;
	/*
		Bonus Value applied to BaseValue of Attribute. It should never be directly modified outside of AbilityFramework.
	*/
	UPROPERTY(SaveGame)
		float BaseBonusValue;

	UPROPERTY(EditAnywhere, SaveGame)
		float MinValue;

	UPROPERTY(EditAnywhere, SaveGame)
		float MaxValue;
	/*
		Bonus to the MaxValue.
	*/
	UPROPERTY(SaveGame)
		float BonusMaxValue;

	UPROPERTY(SaveGame)
		float CurrentValue;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		TSubclassOf<class UGAAttributeExtension> ExtensionClass;

	//Considering amount of bonuses TArray might be better than TMap, for cache coherency.
	TArray<TMap<FGAEffectHandle, FGAEffectMod>> Modifiers;
	FAFAttributeBase();
	FAFAttributeBase(float BaseValueIn);
	void InitializeAttribute(UAFAbilityComponent* InComponent, const FName InAttributeName);
	void CopyFromOther(FAFAttributeBase* Other);
	/* You should never use those tree function to set attributes.
	Only use them for testing/debugging and setting initial values for attributes. */
	inline void SetBaseValue(float ValueIn) { BaseValue = ValueIn; }
	inline void SetMinValue(float ValueIn) { MinValue = ValueIn; }
	inline void SetMaxValue(float ValueIn) { MaxValue = ValueIn; }
	//used internally. NEver call it directly.
	inline void SetCurrentValue(float ValueIn) { CurrentValue = ValueIn; }

	/*
		Gets current absolute maximum of value of the attribute.
		BaseValue + BonusValue, clamped by MaxValue.
	*/
	inline float GetFinalValue()
	{
		return FMath::Clamp<float>(BaseValue + BaseBonusValue, MinValue, MaxValue);
	};
	inline float GetCurrentValue() { return CurrentValue; };
	void CalculateBonus();
	bool CheckIfStronger(const FGAEffectMod& InMod);
	float Modify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty, const FGAEffectContext& InContext);
	void AddBonus(const FGAEffectMod& ModIn, const FGAEffectHandle& Handle);
	void RemoveBonus(const FGAEffectHandle& Handle, EGAAttributeMod InMod);
	void SetExtensionClass(TSubclassOf<class UGAAttributeExtension> InExtensionClass) { ExtensionClass = InExtensionClass; };
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
		TWeakObjectPtr<class UAFAbilityComponent> Causer;
};