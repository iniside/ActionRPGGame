#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.generated.h"

/*
	Explanation of tags from Fred K, on forums:
	https://forums.unrealengine.com/showthread.php?57988-GameplayAbilities-questions&p=220315#post220315
	Here are some examples to illustrate the possible results. Let's assume that our 
	tag container contains exactly one tag, "A.B.C".

	HasTag("A.B.C", Explicit, Explicit) returns true.
	HasTag("A.B", Explicit, Explicit) returns false.
	HasTag("A.B.C.D", Explicit, Explicit) returns false.
	HasTag("E.B.C", Explicit, Explicit) returns false.
	HasTag("A.B.C", IncludeParentTags, IncludeParentTags) returns true.
	HasTag("A.B", IncludeParentTags, IncludeParentTags) returns true.
	HasTag("A.B.C.D", IncludeParentTags, IncludeParentTags) returns true.
	HasTag("E.B.C", IncludeParentTags, IncludeParentTags) returns false.
	HasTag("A.B.C", IncludeParentTags, Explicit) returns true.
	HasTag("A.B", IncludeParentTags, Explicit) returns true.
	HasTag("A.B.C.D", IncludeParentTags, Explicit) returns false.
	HasTag("E.B.C", IncludeParentTags, Explicit) returns false.
	HasTag("A.B.C", Explicit, IncludeParentTags) returns true.
	HasTag("A.B", Explicit, IncludeParentTags) returns false.
	HasTag("A.B.C.D", Explicit, IncludeParentTags) returns true.
	HasTag("E.B.C", Explicit, IncludeParentTags) returns false.
*/
/*
	I seriosuly need to clean this shit up.
*/
/**/
UENUM()
enum class EGAAttributeMod : uint8
{
	Add, //Value =  Value + X
	Subtract, //Value =  Value - X
	Multiply, //Value =  Value * X
	Divide,//Value =  Value / X
	Set, //Value = X

	Invalid
};

UENUM()
enum class EGAAttributeSource : uint8
{
	Instigator,
	Target
};

UENUM()
enum class EGAAttributeValue : uint8
{
	Base,
	Current,
	Final,
	Bonus
};

UENUM()
enum class EGAEffectType : uint8
{
	Instant,
	Periodic,
	Duration,
	Infinite
};

UENUM()
enum class EGAEffectStacking : uint8
{
	HighestOverride, //only one effect modiging these attributes, will be present
	Replace, //will end previous effect, and replace with new one.
	Restart, //will restart existing effect
	Duration, //will add duration to existing effect
	Intensity, //will add magnitude to existing effect.
	Add, //will simply add new effect

	Invalid
};

UENUM()
enum class EGAAttributeMagCalc : uint8
{
	Add, //Value =  Value + X
	Subtract, //Value =  Value - X
	Multiply, //Value =  Value * X
	Divide,//Value =  Value / X
	PrecentageIncrease, //Value = Value + (Value * X)
	PrecentageDecrease, //Value = Value - (Value * X)

	Invalid
};

UENUM()
enum class EGAMagnitudeCalculation : uint8
{
	Direct, //straight float value, no calculations.
	AttributeBased, //calculate based on attribute Attribute * RestOfEquationToBeDecided
	CurveBased, //Takes value of attribute, and then gets value from curve based on this attribute.
	CustomCalculation,//uses custom object to calculate magnitude.
	Invalid
};
/*
	hmmm ? Idk,  but I need some way, to determine, if this modifier from stack
	should affect, incoming effect or not.
*/
UENUM()
enum class EGAModifierDirection : uint8
{
	Incoming,
	Outgoing
};

/*
Rules for where we should aggregate effects.
Concept might look bit muddy at first look, but it is actually very simple.

Let's say we have additive effect which modify Health by 50 points, and effect which modify health by 100.
We want, to only the highest one, affect target (positive bonus). Regardless of who applied this effect.

To do it we need to tell this effect, that it should be aggregated by target and set effect stacking rule to
HighestOverride.

Now say we have effect which reduce Health by 30 points, and second which reduce by 20 points.
We might want negative effects from diffrent source to stack.
To do this we need to aggregate those effects by Instigator.
And then we can decide how those effects will stack within single Instigator.
*/
UENUM()
enum class EGAEffectAggregation : uint8
{
	/*
	Effect will be stacked/aggregated by Instigator who applied it.
	Checking for stacking rules will be done only against other effects from the same Instigator.
	*/
	AggregateByInstigator,
	/*
	Effects will be stacked/aggregated by Target.
	Checking for stacking rules, will be done only for effects, with the same target.
	*/
	AggregateByTarget
};



USTRUCT(BlueprintType)
struct FGAttributeMagnitudeData: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** Preculculated magnitude associated with this attribite level. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Magnitude)
		float Magnitude;

	FGAttributeMagnitudeData()
	{}
};
USTRUCT(BlueprintType)
struct FGAEffectHandle
{
	GENERATED_USTRUCT_BODY()
protected:
	UPROPERTY()
		int32 Handle; //Fname Guid ?

public:
	FGAEffectHandle()
		: Handle(INDEX_NONE)
	{}
	FGAEffectHandle(int32 HandleIn)
		: Handle(HandleIn)
	{}
	bool IsValid()
	{
		return Handle != INDEX_NONE;
	}

	static FGAEffectHandle GenerateHandle();

	bool operator==(const FGAEffectHandle& Other) const
	{
		return Handle == Other.Handle;
	}
	bool operator!=(const FGAEffectHandle& Other) const
	{
		return Handle != Other.Handle;
	}
	friend uint32 GetTypeHash(const FGAEffectHandle& InHandle)
	{
		return InHandle.Handle;
	}

};

/*
	Struct representing final modifier applied to attribute.
*/
USTRUCT(BlueprintType)
struct FGAModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EGAAttributeMod AttributeMod;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Value;

	FGAModifier()
	{};
	FGAModifier(EGAAttributeMod ModIn, float ValueIn)
		: AttributeMod(ModIn),
		Value(ValueIn)
	{};
};
/**
 *	Struct representing single attribute. Needed for Pin customization.
 *	What we really need is to use this struct for making "complex" attributes.
 *	Which means attributes, which needs to track their own state.
 *	Bonuses applied to them, types of bonues, and who applied those bonuses
 *	so we can properly remove them, get them, track them, and controll order
 *	in which theyare added.
 *	we will two have attributes types. Staless (transient), auxiallry attttributes
 *	and staefull attributes, which are going to track their state.
 */
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttribute
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FName AttributeName;

	inline bool operator== (const FGAAttribute& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName == AttributeName);
	}

	inline bool operator!= (const FGAAttribute& OtherAttribute) const
	{
		return (OtherAttribute.AttributeName != AttributeName);
	}

	inline bool IsValid() const
	{
		return !AttributeName.IsNone();
	}

	FGAAttribute()
	{
		AttributeName = NAME_None;
	};
	FGAAttribute(const FName& AttributeNameIn)
	{
		AttributeName = AttributeNameIn;
	};
};

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
protected:
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
	TMap<FGAEffectHandle, FGAModifier> Modifiers;

public:
	inline void SetBaseValue(float ValueIn){ BaseValue = ValueIn; }
	inline float GetFinalValue()
	{ 
		return FMath::Clamp<float>(BaseValue + BonusValue, 0,ClampValue);
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

	inline bool operator== (const FGAAttributeBase& OtherAttribute) const
	{
		return (OtherAttribute.CurrentValue == CurrentValue);
	}

	inline bool operator!= (const FGAAttributeBase& OtherAttribute) const
	{
		return (OtherAttribute.CurrentValue != CurrentValue);
	}

	FGAAttributeBase()
		: BaseValue(0),
		BonusValue(0)
	{
	};
	FGAAttributeBase(float BaseValueIn)
		: BaseValue(BaseValueIn),
		BonusValue(0)
	{
	};
};

USTRUCT(BlueprintType)
struct FGAEffectContext
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	 *	Where exactly we hit target.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FVector TargetHitLocation;
	/**
	 *	Direct Reference to TargetActor (I will possibly remove FHitResult Target!
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<AActor> Target;
	/**
	 *	Object which caused this effect. Might be ability, weapon, projectile etc.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<UObject> Causer;
	/**
	 *	Pawn, which originally instigated effect (an owned AttributeComponent).
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<APawn> Instigator;
	/**
	 *	Attribute component of Target.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributeComponent> TargetComp;
	/**
	 *	Attribute component of Intigator
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributeComponent> InstigatorComp;

	void Reset();

	FGAEffectContext()
	{}

	FGAEffectContext(const FVector& TargetHitLocationIn, TWeakObjectPtr<AActor> TargetIn,
					TWeakObjectPtr<UObject> CauserIn, TWeakObjectPtr<APawn> InstigatorIn,
					TWeakObjectPtr<class UGAAttributeComponent> TargetCompIn,
					TWeakObjectPtr<class UGAAttributeComponent> InstigatorCompIn)
					: TargetHitLocation(TargetHitLocationIn),
						Target(TargetIn),
						Causer(CauserIn),
						Instigator(InstigatorIn),
						TargetComp(TargetCompIn),
						InstigatorComp(InstigatorCompIn)
	{}

	~FGAEffectContext();
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


/*
	This struct contains information needed for effect, to modify attribute
	on other effects,
*/
USTRUCT(BlueprintType)
struct FGAEffectModifier
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		How this modifier should stack.
	*/
	UPROPERTY(EditAnywhere)
		EGAEffectStacking Stacking;
	/*
		Where is should stack (currently have no effect).
	*/
	UPROPERTY(EditAnywhere)
		EGAEffectAggregation Aggregation;
	/*
		What kind of modifier, we are going to apply.
	*/
	UPROPERTY(EditAnywhere)
		EGAAttributeMod Mod;
	/*
		Should this modifier, modify incoming, or outgoing effects (if applicable).
		I should probabaly move effect modification into separate struct. We don't really need it here.
		and It can encapsluate more than simple mods. but for testing..
	*/
	UPROPERTY(EditAnywhere)
		EGAModifierDirection ModDirection;
	/*
		Kind of calculation, we will use to determine this modifier.
	*/
	UPROPERTY(EditAnywhere)
		EGAMagnitudeCalculation CalculationType;

	UPROPERTY(EditAnywhere)
		FGADirectModifier DirectModifier;

	/*
		I require all of these tags, on effect to be applied to it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		All of these tags must be present actor target, which will be affected, by
		this modified effect (these tags, are not going to be checked directly
		against effect itself, only against effect target).
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer TargetRequiredTags;
	/*
		All of these tags must be present on instigator target, which will be affected, by
		this modified effect (these tags, are not going to be checked directly
		against effect itself, only against effect target).
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer InstigatorRequiredTags;

	float GetFinalValue();
};

/*
	Group modifiers, for this attribute and this Tag in single struct.
*/
USTRUCT(BlueprintType)
struct FGEffectModifierGroup
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Attribute other effect modify, which we will modify on this effect.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
		Only one tag, per one modifier, so we have EXACT match.
		If you want more generic modifier, you just need to use Tag higher in hierarchy.
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTag AttributeTag;
	UPROPERTY(EditAnywhere)
		TArray<FGAEffectModifier> Modifiers;

};
USTRUCT()
struct FGACalculatedEffectModifier
{
	GENERATED_USTRUCT_BODY()
};

/*
	Contains definitions for specific modifer we want to override;
*/
USTRUCT(BlueprintType)
struct FGAModifierOverride
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Attribute associated with modifier.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGAAttribute Attribute;
	/*
		New modifier we want to apply.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FGAAttributeModifier Modifier;
};

USTRUCT(BlueprintType)
struct FGAEffectDuration
{
	GENERATED_USTRUCT_BODY()
};

USTRUCT(BlueprintType)
struct FGAEffectPolicy
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		EGAEffectType Type;
	UPROPERTY(EditAnywhere)
		EGAEffectStacking Stacking;
	UPROPERTY(EditAnywhere)
		EGAEffectAggregation Aggregation;
};
/*
	1. If spec is instant application, we will try to apply it directly without any fuss.
	2. If it have duration (or is infinite), we will create FGAActiveEffect out of it.
*/
USTRUCT(BlueprintType)
struct FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectSpec() {};
	FGAEffectSpec(const FGAEffectContext& ContextIn) 
		: Context(ContextIn)
	{};

	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn, const FGAEffectContext& ContextIn);
	/*
		These constructors are used with conjuction with static functions,
		to easily alter parts of effect spec, without spawning/chagning 
		UGAEffectSpecification, which can provide some defaults.
	*/
	/*
		Constructor which will override duration.
	*/
	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
		FGAEffectDuration DurationIn, const FGAEffectContext& ContextIn);
	/*
		Constructor which will override entire stack of modifiers.
	*/
	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
		TArray<FGAAttributeModifier> ModifiersIn, const FGAEffectContext& ContextIn);
	/*
		Constructor which will override only specific modifiers.
	*/
	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
		TArray<FGAModifierOverride> OverridesIn, const FGAEffectContext& ContextIn);
	
	UPROPERTY()
		const UGAEffectSpecification* EffectSpec;

	/*
		These properties will mirror the ones in GAEffectSpecification.
		We use GAEffectSpecification as template, which is used to initialize, those properties
		and then we can override them using various static functions.

		Or we can just apply them directly.

		EditAnywhere, so we can create an inline spec, when we need it.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> AttributeModifiers;

	/*
		Modifiers which will be applied on effect period.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> PeriodModifiers;
	/*
		Modifiers which will be applied when effect is prematurly removed
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> RemovedModifiers;
	/*
		Modifiers which will be applied when effect naturally expired.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> ExpiredModifiers;

	/*
		If I have duration, I will modify other effects, attribute modifiers,
		if those effects meet these requirements.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGEffectModifierGroup> EffectModifiers;

	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectDuration EffectDuration;

	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTag MyTag;
	/*
		I require these tags on target to be applied.
		If this is empty I will ignore it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		I will apply these tags, to target if I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer GrantedTags;
	/*
		I will add these immunity tags, to target if I'm applied.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer GrantedImmunityTags;
	/*
		I require any of these tags, on other effect
		to be able to modify it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OtherEffectRequire;


	/*
		I will do something when I expire, If target have these tags.
		
		Ok. It's kind of dumb, I should do it differently, we need more data
		about who is target (target doesn't need to be actor, to which
		effect is applied, but for example, actor, who hit actor with this effect.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ExpiredRequireTags;

	UPROPERTY()
		FGAEffectContext Context;

	TArray<FGAAttributeData> GetModifiers();

	TArray<FGAAttributeData> EvalModifiers;

	TArray<FGAAttributeData> GetPeriodModifiers();

	TArray<FGAAttributeData> PeriodMods;
};

USTRUCT()
struct FGACountedTagContainer
{
	GENERATED_USTRUCT_BODY()
protected:
	/*
		Here we will store counted GamaplayTags. 
		If tag is already in map, we just incremement count.
	*/
	TMap<FGameplayTag, int32> CountedTags;

	/*
		Here we store all currently posesd tags.
		It is equivalent of CountedTags, except this does not track count of tags, but we need it
		to have something against which we can perform queries.
	*/
	FGameplayTagContainer AllTags;
public:
	void AddTag(const FGameplayTag& TagIn);
	void AddTagContainer(const FGameplayTagContainer& TagsIn);
	void RemoveTag(const FGameplayTag& TagIn);

	bool HasTag(const FGameplayTag& TagIn);
	bool HasAllTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);
	bool HasAnyTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);
};

USTRUCT()
struct FGAEffectInstant
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectInstant()
	{}
	FGAEffectInstant(const FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn);

	TArray<FGAAttributeData> Modifiers;
	//FGAEffectContext Context;
	/*
		In't not going to look like that, but the general idea, that this container
		is goint to have all accumulated tags, from spec, and also from Causer, Instigator, Target
	*/
	FGameplayTagContainer AccumulatedTags;
};

USTRUCT()
struct FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
	FGAEffectHandle MyHandle;
	UPROPERTY()
	FGAEffectContext Context;

	TArray<FGameplayTag> AttributeEffectModifiers;

	TArray<FGEffectModifierGroup> EffectModifiers;
public:
	virtual void ActivateEffect() {};
	virtual void FinishEffect() {};
};

USTRUCT()
struct FGAActiveDuration : public FGAActiveBase
{
	GENERATED_USTRUCT_BODY()

	void OnApplied() {};
	void OnRemoved() {};
	void OnEnded() {};
protected:
	FTimerHandle DurationTimerHandle;
};

USTRUCT()
struct FGAActivePeriodic : public FGAActiveDuration
{
	GENERATED_USTRUCT_BODY()
public:
	void OnPeriod();
	void OnApplied() {};
	void OnRemoved() {};
	void OnEnded();
protected:
	FTimerHandle PeriodTimerHandle;

	TArray<FGAAttributeData> PeriodModifiers;

public:
	virtual void ActivateEffect() override;
	virtual void FinishEffect() override;
	FGAActivePeriodic()
	{}

	FGAActivePeriodic(const FGAEffectContext& ContextIn)
	//:	Context(ContextIn)
	{
		Context = ContextIn;
	}
	FGAActivePeriodic(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn, 
		const FGAEffectHandle& HandleIn);

	~FGAActivePeriodic();
};

USTRUCT()
struct FGAActiveInfinite : public FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
public:
};


USTRUCT()
struct FGACalculatedModifier
{
	GENERATED_USTRUCT_BODY()
public:
	float OutgoingAddtiveMod;
	float OutgoingSubtractMod;
	float OutgoingMultiplyMod;
	float OutgoingDivideMod;

	float IncomingAddtiveMod;
	float IncomingSubtractMod;
	float IncomingMultiplyMod;
	float IncomingDivideMod;
};

USTRUCT()
struct FGATaggedModifiers
{
	GENERATED_USTRUCT_BODY()
public:
	FGameplayTagContainer RequiredTargetTags;
	FGameplayTagContainer RequiredInstigatorTags;

	TMap<FGAEffectHandle, TArray<FGAEvalData>> EffectMods;
	void CalculateNewModifierStack();
	void AddModifierEffect(const FGAEffectHandle& HandleIn, const FGAEvalData& Eval);
	void RemoveModifierEffect(const FGAEffectHandle& HandleIn);
	int32 GetModifierCount();
	float OutgoingAddtiveMod;
	float OutgoingSubtractMod;
	float OutgoingMultiplyMod;
	float OutgoingDivideMod;

	float IncomingAddtiveMod;
	float IncomingSubtractMod;
	float IncomingMultiplyMod;
	float IncomingDivideMod;
};

/*
	Group modifiers for single attribute tag by:
	1. Just attribute. No special requriments.
	2. By tags. 
*/
USTRUCT()
struct FGAActiveEffectsModifiers
{
	GENERATED_USTRUCT_BODY()
public:
	FGAActiveEffectsModifiers()
		: OutgoingAddtiveMod(0),
		OutgoingSubtractMod(0),
		OutgoingMultiplyMod(0),
		OutgoingDivideMod(1)
	{}
	//Modifiers, which have tag requirments
	TArray<FGATaggedModifiers> TaggedModifiers;
protected:
	/*
		Group array of modifiers, by handle to effect,
		so we know, which modifier were applied by which effect
		so we can easily remove them along effect.
	*/
	TMap<FGAEffectHandle, TArray<FGAEvalData>> EffectMods;
	//requiredTag, Mods
	TMap<FGameplayTag, FGATaggedModifiers> mods;

	//TMap<FGAEffectHandle, TArray<FGATaggedModifiers>> TaggedEffectMods;
	/*
		Base mods applicable to everything.
	*/
	/**/
	float OutgoingAddtiveMod;
	float OutgoingSubtractMod;
	float OutgoingMultiplyMod;
	float OutgoingDivideMod;

	float IncomingAddtiveMod;
	float IncomingSubtractMod;
	float IncomingMultiplyMod;
	float IncomingDivideMod;

public:
	
	//FGameplayTagContainer RequiredTargetTags;
	//FGameplayTagContainer RequiredInstigatorTags;
	void AddTaggedModifier(const FGameplayTagContainer& RequiredTargetTags,
		const FGameplayTagContainer& RequiredInstigatorTags,
		const FGAEffectHandle& HandleIn, const FGAEvalData& Eval);
	void ApplyModifiers(FGAAttributeData& EvalData, const FGAEffectContext& Ctx);
	void AddModifierEffect(const FGAEffectHandle& HandleIn, const FGAEvalData& Eval);
	void RemoveModifierEffect(const FGAEffectHandle& HandleIn);

	int32 GetModifierCount();
	//recalculates entire stack, every time effect is added/or removed.
	void CalculateNewModifierStack();

};

USTRUCT()
struct FGAEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
};

USTRUCT()
struct FGAActiveEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	FGAActiveEffectContainer()
	{}
	FGAEffectHandle ApplyEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Ctx);

	void RemoveActiveEffect(const FGAEffectHandle& HandleIn);

	void ApplyEffectModifiers(FGAAttributeData& DataIn, const FGAEffectContext& Ctx);

	/*
		Execute modifiers from existing effect, spec on incoming effect.
	*/
	void ExecuteEffectModifier(FGAAttributeData& ModifierIn, const FGAEffectContext& Ctx);

	FGAEffectHandle HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx);

	FGAEffectHandle HandlePeriodicEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	FGAEffectHandle HandleDurationEffect(FGAActiveBase& EffectIn, const FGAEffectContext& Ctx);

	FGAEffectHandle HandleInfiniteEffect(const FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	void RemoveInfiniteEffect(const FGAEffectHandle& HandleIn);

	void Clean();

	/*
		map of active effects, with limited duration.
		Store as shared pointers, so we can have some basic polymorphism.

		Not, extremly important and might change later!
	*/
	TMap<FGAEffectHandle, TSharedPtr<FGAActiveBase>> ActiveEffects;


	/*
		Modifiers, from effects, grouped by Handle.
	*/
	TMap<FGAEffectHandle, TArray<FGAEffectModifier>> EffectModifiers;

	/*
		Use it to store modifier tags (tags, from effects which modify other effect),
	*/
	FGACountedTagContainer ActiveModifierTags;
	/*
		Active modifiers for this tag.
		Hm. Name is misleading, since, those modifiers, are able, to only 
		modify, attribute modifiers inside effect.

		If effect have any special logic, it's unmoddable.

		We need mechanism, which will let as mute effects (to some degree).
		By that I mean replace tags and/or change affected attribute.

		For example we might want to change damage type from Damage.Fire to Damage.Ice
		(for whatever reason).

		Or we might want to change attribute from Damage, To healing (so all incoming damage
		will heal us instead). Etc.
		
		We are grouping active modifiers, by AttributeTag.

		Like Damage.Fire, Damage.Ice,
		Downside. if incoming tag is Damage it will not modify Damage.Fire, while it should.
	*/
	TMap<FGameplayTag, FGAActiveEffectsModifiers> Modifiers;
	/*
		Group modifiers, of single attribute into Array.
	*/
	TMap<FGameplayTag, TArray<FGAActiveEffectsModifiers>> AttributeModifiers;

	TMap<FGAEffectHandle, TSharedPtr<FGAActiveInfinite>> InfiniteEffects;

	/*
		Map grouping effect handles from instigators (???)
	*/
	TMap<TWeakObjectPtr<class UGAAttributeComponent>, TArray<FGAEffectHandle>> InstigatorEffects;
};


