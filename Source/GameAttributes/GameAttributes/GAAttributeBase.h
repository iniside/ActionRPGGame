#pragma once
#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAAttributeBase.generated.h"
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
struct GAMEATTRIBUTES_API FGAAttributeBase
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
protected:
	UPROPERTY()
		float AdditiveBonus;
	UPROPERTY()
		float SubtractBonus;
	UPROPERTY()
		float MultiplyBonus;
	UPROPERTY()
		float DivideBonus;
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
	TMap<FGAGameEffectHandle, TArray<FGAModifier>> Modifiers;

public:
	/*
		Return value of additive bonus, without BaseValue.
	*/
	inline float GetAdditiveBonus() { return AdditiveBonus; }
	/*
		Return value of subtract bonus, without BaseValue.
	*/
	inline float GetSubtractBonus() { return SubtractBonus; }
	/*
		Return value of Multiply bonus, without BaseValue.
	*/
	inline float GetMultiplyBonus() { return MultiplyBonus; }
	/*
		Return value of Divide bonus, without BaseValue.
	*/
	inline float GetDivideBonus() { return DivideBonus; }

	inline void SetBaseValue(float ValueIn){ BaseValue = ValueIn; }
	inline float GetFinalValue()
	{
		return FMath::Clamp<float>(BaseValue + BonusValue, 0, ClampValue);
	};
	inline float GetCurrentValue(){ return CurrentValue; };
	void UpdateAttribute();

	void Add(float ValueIn);
	void Subtract(float ValueIn);
	void ApplyDurationModifier(const FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn) {};

	//inline float GetCurrentValue()
	//{
	//	return FMath::Clamp<float>(BaseValue + BonusValue + AccumulatedBonus, 0, GetFinalValue());;
	//}
	/*
		Use it to apply instant, permament change to attribute.
		Don't use it with multiply/Divide!
	*/
	void InstantApplication(const FGAModifier& ModifierIn);

	void AddBonus(const FGAModifier& ModifiersIn, const FGAGameEffectHandle& Handle
		, EGAEffectStacking StackingType);
	void RemoveBonus(const FGAGameEffectHandle& Handle);
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

	//inline bool operator== (const FGAAttributeBase& OtherAttribute) const
	//{
	//	return (OtherAttribute.CurrentValue == CurrentValue);
	//}

	//inline bool operator!= (const FGAAttributeBase& OtherAttribute) const
	//{
	//	return (OtherAttribute.CurrentValue != CurrentValue);
	//}

	FGAAttributeBase()
		: BaseValue(0),
		BonusValue(0),
		AdditiveBonus(0),
		SubtractBonus(0),
		MultiplyBonus(0),
		DivideBonus(0),
		CurrentValue(BaseValue)
	{
	};
	FGAAttributeBase(float BaseValueIn)
		: BaseValue(BaseValueIn),
		BonusValue(0),
		AdditiveBonus(0),
		SubtractBonus(0),
		MultiplyBonus(0),
		DivideBonus(0),
		CurrentValue(BaseValue)
	{
	};
};

/*
Evaluated attribute data from effects. It can still change, at Attribute object level!.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEvalData
{
	GENERATED_USTRUCT_BODY()
public:
	//attribute we changed
	UPROPERTY(BlueprintReadOnly)
		FGAAttribute Attribute;
	//How we intend to modify attribute
	UPROPERTY(BlueprintReadOnly)
		EGAAttributeMod Mod;
	UPROPERTY(BlueprintReadOnly)
		EGAModifierDirection ModDirection;
	//associated tag.
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag AttributeTag;
	//final value we will try to apply to attribute.
	UPROPERTY(BlueprintReadOnly)
		float Value;
	/*
		Useful for calculating final bonuses, in attribute class, where we can easily
		get all modifiers from causer and target.
	*/
	/*
		Who caused, to whom.
	*/
	UPROPERTY()
		FGAEffectContext Context;

	FGAEvalData()
	{
	};
	FGAEvalData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		const FGameplayTag& AttributeTagIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(EGAModifierDirection::Outgoing),
		AttributeTag(AttributeTagIn),
		Value(ValueIn)
	{
	};
	FGAEvalData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		EGAModifierDirection ModDirectionIn, const FGameplayTag& AttributeTagIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(ModDirectionIn),
		AttributeTag(AttributeTagIn),
		Value(ValueIn)
	{
	};
	FGAEvalData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		EGAModifierDirection ModDirectionIn, const FGameplayTag& AttributeTagIn,
		const FGAEffectContext& ContextIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(ModDirectionIn),
		AttributeTag(AttributeTagIn),
		Context(ContextIn),
		Value(ValueIn)
	{
	};
};

/*
Evaluated, attribute data, containing tags, atribute name, mod type etc.
We use it, to apply attribute change, and to verify if this data can be modified on the
way by other effects.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeData
{
	GENERATED_USTRUCT_BODY()
public:
	//attribute we changed
	UPROPERTY(BlueprintReadOnly)
		FGAAttribute Attribute;

	//How we intend to modify attribute
	UPROPERTY(BlueprintReadOnly)
		EGAAttributeMod Mod;

	EGAModifierTarget Target;

	UPROPERTY(BlueprintReadOnly)
		FGameplayTag AttributeTag;
	/*
		Aggregated tags, from effect & target
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer AgreggatedTags;
	/*
		These tags, must be present on target.
		Ignored if empty.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer TargetTagsRequiared;
	/*
		These tags, are must be present on instigator.
		Ignored if empty.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer InstigatorTagsRequiared;

	/*
		All of these tags must be present on target for this change to be applied.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer RequiredTags;
	/*
		None of these tags might be present on target for this change to be applied.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer DenyTags;
	UPROPERTY()
		FGAEffectContext Context;

	//final value we will try to apply to attribute.
	UPROPERTY(BlueprintReadOnly)
		float Value;

	inline bool operator>(const FGAAttributeData& OtherIn) const
	{
		return OtherIn.Value > Value;
	}
	inline bool operator<(const FGAAttributeData& OtherIn) const
	{
		return OtherIn.Value < Value;
	}

	FGAAttributeData()
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		const FGameplayTagContainer& AgreggatedTagsIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		AgreggatedTags(AgreggatedTagsIn),
		Value(ValueIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		const FGameplayTag& AttributeTagIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		AttributeTag(AttributeTagIn),
		Value(ValueIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		Value(ValueIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn, float ValueIn, const FGAEffectContext ContextIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		Value(ValueIn),
		Context(ContextIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		float ValueIn, const FGAEffectContext& ContextIn,
		EGAModifierTarget TargetIn, const FGameplayTagContainer& RequiredTagsIn,
		const FGameplayTagContainer& DenyTagsIn)
		:Attribute(AttributeIn),
		Mod(ModIn),
		Value(ValueIn),
		Context(ContextIn),
		Target(TargetIn),
		RequiredTags(RequiredTagsIn),
		DenyTags(DenyTagsIn)
	{};
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAModifiedAttribute
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
		TWeakObjectPtr<class UGAAttributeComponent> Causer;
};

/*
It will somehow calculcate magnitude out of something.
*/
USTRUCT(BlueprintType)
struct FGAModifierMagnitude
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	Source of Attribute for this calculation.
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeSource Source;
	/*
	Name of attribute used in calculation.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
	Curve we will use for calculation.
	*/
	UPROPERTY(EditAnywhere)
		FCurveTableRowHandle CurveTable;

	float GetMagnitude(const FGAEffectContext& Context);
};