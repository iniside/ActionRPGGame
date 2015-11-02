#pragma once
#include "GameAttributes.h"
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.generated.h"

/* Final calculcated mod from effect, which can be modified by Calculation object. */
struct FGAEffectMod
{
	FGAAttribute Attribute;
	float Value;
	EGAAttributeChangeType ChangeType;

	/*
	Spec from which this mod has been derived.
	Used to do not copy to much heavy data around.
	*/
	class UGAGameEffectSpec* EffectSpec;

	FGAEffectMod()
		: Attribute(NAME_None),
		Value(0),
		ChangeType(EGAAttributeChangeType::Invalid)
	{};

	FGAEffectMod(const FGAAttribute& AttributeIn, float ValueIn,
		EGAAttributeChangeType ChangeTypeIn, class UGAGameEffectSpec* EffectSpecIn)
		: Attribute(AttributeIn),
		Value(ValueIn),
		ChangeType(ChangeTypeIn),
		EffectSpec(EffectSpecIn)
	{};
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

	inline float GetValue() { return Value; };
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

	float GetValue(const FGAEffectContext& ContextIn);
};