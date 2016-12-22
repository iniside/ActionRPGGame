#pragma once
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAEffectGlobalTypes.h"
#include "GAAttributeBase.generated.h"

DECLARE_MULTICAST_DELEGATE(FGAGenericAttributeDelegate);

DECLARE_STATS_GROUP(TEXT("Attribute"), STATGROUP_Attribute, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("CalculateBonus"), STAT_CalculateBonus, STATGROUP_Attribute, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("CurrentBonusByTag"), STAT_CurrentBonusByTag, STATGROUP_Attribute, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("FinalBonusByTag"), STAT_FinalBonusByTag, STATGROUP_Attribute, );

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
struct GAMEABILITIES_API FGAAttributeBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		float BaseValue;
	/*
	This is maxmum value of this attribute.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		float ClampValue;
	/*
		Current Value. BaseValue + BonusValue - AnyDamageIhave Takend, Clamped between 0 and ClampValue.
	*/
	UPROPERTY(BlueprintReadOnly)
		float CurrentValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Value")
		TSubclassOf<class UGAAttributeExtension> ExtensionClass;
protected:
	/*
		Bonus value calculated from stack of affecting effects.
	*/
	UPROPERTY()
		float BonusValue;
public:
	//map of modifiers.
	//It could be TArray, but map seems easier to use in this case
	//we need to keep track of added/removed effects, and see 
	//if this effect affected this attribute.
	//hmp. Technically, one effect, can provide multiple modifiers
	//like periodic.
	TMap<FGAEffectHandle, FGAModifier> Modifiers;

public:
	inline void SetBaseValue(float ValueIn){ BaseValue = ValueIn; }
	inline void SetClampValue(float ValueIn) { ClampValue = ValueIn; }
	inline float GetFinalValue()
	{
		return FMath::Clamp<float>(BaseValue + BonusValue, 0, ClampValue);
	};
	inline float GetCurrentValue(){ return CurrentValue; };
	void UpdateAttribute();
	float Modify(const FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn);
	void Add(float ValueIn);
	void Subtract(float ValueIn);
	void ApplyDurationModifier(const FGAEffectHandle& HandleIn, FGAEffectMod& ModIn) {};
	void Tick() {};
	//inline float GetCurrentValue()
	//{
	//	return FMath::Clamp<float>(BaseValue + BonusValue + AccumulatedBonus, 0, GetFinalValue());;
	//}
	/*
		Use it to apply instant, permament change to attribute.
		Won't work with Multiply/Divide.
	*/
	void InstantApplication(const FGAModifier& ModifierIn);
	bool CheckIfStronger(const FGAEffectHandle& HandleIn);
	void AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle
		, EGAEffectStacking StackingType);
	void RemoveBonus(const FGAEffectHandle& Handle);
	/*
		Removes bonus from this attribute based on it's type.
	*/
	void RemoveBonusByType(EGAAttributeMod ModType);
	/*
		Remove weaker bonus of the same mod type, from Modifiers.
	*/
	void RemoveWeakerBonus(EGAAttributeMod ModType, float ValueIn);
	/*
		Removes bonus of the same mod type from Modifiers.
	*/
	void RemoveBonusType(EGAAttributeMod ModType);

	void InitializeAttribute();

	void CalculateBonus();

	float GetCurrentValueByTags(const FGameplayTagContainer& TagsIn, FGAIndividualMods& Bonuses) const;
	float GetFinalValueByTags(const FGameplayTagContainer& TagsIn, FGAIndividualMods& Bonuses) const;
	float GetBonusValueByTags(const FGameplayTagContainer& TagsIn, FGAIndividualMods& Bonuses) const;
	//inline bool operator== (const FGAAttributeBase& OtherAttribute) const
	//{
	//	return (OtherAttribute.CurrentValue == CurrentValue);
	//}

	//inline bool operator!= (const FGAAttributeBase& OtherAttribute) const
	//{
	//	return (OtherAttribute.CurrentValue != CurrentValue);
	//}

	FGAAttributeBase()
		: CurrentValue(0),
		BonusValue(0)
	{
	};
	FGAAttributeBase(float BaseValueIn)
		: BaseValue(BaseValueIn),
		CurrentValue(BaseValue),
		BonusValue(0)
	{
	};
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGAModifiedAttribute
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