#pragma once
#include "GAGlobalTypes.h"
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
	TMap<FGAEffectHandle, TArray<FGAModifier>> Modifiers;

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

	//inline float GetCurrentValue()
	//{
	//	return FMath::Clamp<float>(BaseValue + BonusValue + AccumulatedBonus, 0, GetFinalValue());;
	//}
	/*
		Use it to apply instant, permament change to attribute.
		Don't use it with multiply/Divide!
	*/
	void InstantApplication(const FGAModifier& ModifierIn);

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

	UPROPERTY(BlueprintReadOnly)
		FGameplayTag AttributeTag;
	/*
		Aggregated tags, from effect & target
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer AgreggatedTags;
	/*
		These tags, are must be present on target.
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
Base concept behind those attributes calculations (not sure if these are any good solutions
/performance firendly but:
1. Prefer getting values directly of attributes (GetBaseValue(), GetFinalValue(), GetBonus() etc),
as attributes can track their own state, we don't need any sophisticated way to access them and calculate bonuses).
2. Prefer simple and direct formulas (either evaluate from CurveTable, just plain add, Multiply various numbers,
or get value directly).
3. This system is not inteded, to calculate absolute final value on effect, before that effect is applied.
It will calculate absolute maximum value from the informations it have access to.
Further modifications like increasing specific damage type value, or reducing damage by value, are done
on AttributeComponent, by implementing approperiate functions, or by existing effects, which happen to intercept
incoming effect.
This might change in future though. But even if, I still prefer the magnitude calculations to be simple,
and we will add needed buffs/debuffs as we progress trough execution chain.
4. All calculations are linear.
*/
//EGAMagnitudeCalculation::Direct
USTRUCT(BlueprintType)
struct FGADirectModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		float Value;

	inline float GetValue(){ return Value; };
};
//EGAMagnitudeCalculation::AttributeBased
USTRUCT(BlueprintType)
struct FGAAttributeBasedModifier
{
	/*
	We also need to add concept of backing attributes,
	though I'm not sure how I want to handle them at this point.
	*/
	GENERATED_USTRUCT_BODY()
public:
	/*
	Source of Attribute for this calculation.
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeSource Source;
	/*
	Name of attribute Used for calculation.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, meta = (FixedIncrement = "0.01"))
		float Coefficient;
	UPROPERTY(EditAnywhere, meta = (FixedIncrement = "0.01"))
		float PreMultiply;
	UPROPERTY(EditAnywhere, meta = (FixedIncrement = "0.01"))
		float PostMultiply;
	UPROPERTY(EditAnywhere, meta = (FixedIncrement = "0.01"))
		float PostCoefficient;
	/*
	Should we use secondary attribute for this ecalculation ?
	*/
	UPROPERTY(EditAnywhere)
		bool bUseSecondaryAttribute;
	/*
	Source for secondary attribute
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeSource SecondarySource;
	/*
	Name of secondary attribute used in calculation.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute SecondaryAttribute;
	/*
	What shouldd we do with secondary attribute ?

	case EGAAttributeMagCalc::Add:
	return BaseResult + attrValue;
	case EGAAttributeMagCalc::Subtract:
	return BaseResult - attrValue;
	case EGAAttributeMagCalc::Multiply:
	return BaseResult * attrValue;
	case EGAAttributeMagCalc::Divide:
	return BaseResult / attrValue;
	case EGAAttributeMagCalc::PrecentageIncrease:
	return BaseResult + (BaseResult * attrValue);
	case EGAAttributeMagCalc::PrecentageDecrease:
	return BaseResult - (BaseResult * attrValue);
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeMagCalc SecondaryMod;

	FGAAttributeBasedModifier()
		: Source(EGAAttributeSource::Instigator),
		Coefficient(1),
		PreMultiply(0),
		PostMultiply(0),
		PostCoefficient(1),
		bUseSecondaryAttribute(false)
	{}

	float GetValue(const FGAEffectContext& Context);
};


//EGAMagnitudeCalculation::CurveBased
USTRUCT(BlueprintType)
struct FGACurveBasedModifier
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	Source of Attribute for this calculation.
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeSource Source;
	/*
	Name of attribute from which we will take XValue for Curve
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
	Curve and row from which we will take YValue.
	*/
	UPROPERTY(EditAnywhere)
		FCurveTableRowHandle CurveTable;

	float GetValue(const FGAEffectContext& ContextIn);
};
//EGAMagnitudeCalculation::CustomCalculation
USTRUCT(BlueprintType)
struct FGACustomCalculationModifier
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Instanced, so we can setup custom properties this class might provide, per effect spec.
	*/
	UPROPERTY(EditAnywhere, Instanced)
	class UGACustomCalculation* CustomCalculation;
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
/*
	Spec for modifing attribute.
*/
USTRUCT(BlueprintType)
struct FGAAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Type of calculation we want to perform for this Magnitude.
	*/
	UPROPERTY(EditAnywhere)
		EGAMagnitudeCalculation CalculationType;
	/*
		Attribute which will be modified.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
		How Attribute Will be modified
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeMod Mod;

	UPROPERTY(EditAnywhere)
		FGADirectModifier DirectModifier;
	/*
		Simple calculation based on attribute:
		(Coefficient * (PreMultiply + AttributeValue) + PostMultiply) * PostCoefficient

		There is no any magic manipulation, it straight off pull attribute from selected source,
		and make this operation on it.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttributeBasedModifier AttributeBased;
	/*
		Get value from selected CurveTable, based on selected attribute value.
	*/
	UPROPERTY(EditAnywhere)
		FGACurveBasedModifier CurveBased;
	/*
		Provide custom calculation class.
	*/
	UPROPERTY(EditAnywhere)
		FGACustomCalculationModifier Custom;

	/*
		Currently evaluated magnitude. Figured I should store it here, so it can be further modified
		by other effects, or something.
	*/
	FGAAttributeData EvalData;

	FGAAttributeData GetModifier(const FGAEffectContext& ContextIn);
	FGAAttributeModifier()
	{}
};