#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.generated.h"



USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeBase // : public UObject
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
	float BonusValue;
	/*
		Current Value. BaseValue + BonusValue - AnyDamageIhave Takend, Clamped between 0 and ClampValue.
	*/
	float CurrentValue;

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

	void AddBonus(const FGAModifier& ModifiersIn, const FGAEffectHandle& Handle);
	void RemoveBonus(const FGAEffectHandle& Handle);

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
		DivideBonus(1)
	{
	};
	FGAAttributeBase(float BaseValueIn)
		: BaseValue(BaseValueIn),
		BonusValue(0),
		AdditiveBonus(0),
		SubtractBonus(0),
		MultiplyBonus(0),
		DivideBonus(1)
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
		EGAModifierDirection ModDirection;
	UPROPERTY(BlueprintReadOnly)
		FGameplayTag AttributeTag;
	/*
		Tag for this Attribute.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer AttributeTags;
	/*
		These tags, are must be present on target.
		Ignored if empty.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer TargetTagsRequiared;
	/*
		These tags, are must be present on target.
		Ignored if empty.
	*/
	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer InstigatorTagsRequiared;

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
		const FGameplayTagContainer& AttributeTagsIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(EGAModifierDirection::Outgoing),
		AttributeTags(AttributeTagsIn),
		Value(ValueIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		EGAModifierDirection ModDirectionIn, const FGameplayTagContainer& AttributeTagsIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(ModDirectionIn),
		AttributeTags(AttributeTagsIn),
		Value(ValueIn)
	{
	};
	FGAAttributeData(const FGAAttribute& AttributeIn, EGAAttributeMod ModIn,
		EGAModifierDirection ModDirectionIn, const FGameplayTag& AttributeTagIn, float ValueIn)
		: Attribute(AttributeIn),
		Mod(ModIn),
		ModDirection(ModDirectionIn),
		AttributeTag(AttributeTagIn),
		Value(ValueIn)
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
	UPROPERTY()
		float ModifiedByValue;

	/**
	*	Final tags appiled by this change.
	*/
	UPROPERTY()
		FGameplayTagContainer Tags;

	UPROPERTY()
		FVector TargetLocation; //change to vector, we need only position.
	UPROPERTY()
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
		//TSubclassOf<UGACustomCalculation> CalcClass;
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

	EGAModifierDirection ModDirection;
	/*
		Tag for this attribute. Something like Damage.Fire, Damage.Condition.Bleed, Boon.SpeedBonus,
		Damage.Condition.Burning.

		These tags are used to check if this modifier can be modified by other effects.

		I still need to figure out exactly to handle "targeted" bonuses like +20% damage against Undead.

		Deprecated use AttributeTags
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTag AttributeTag;

	/*
		Don't use this at all.. Have another idea!
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer AttributeTags;

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
		: ModDirection(EGAModifierDirection::Outgoing)
	{}
};