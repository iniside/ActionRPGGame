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
	StrongerOverride, //only one effect modifing these attributes, will be present
	Override, //will end previous effect, and replace with new one.
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
	CanStackSameType - will check effects, for their types.
	If matching effect is found, the one with higher modifiers will remain on stack
	and the other one will be removed.

	SameTypeOverride - will check for existing effect modifier.
	If found it will simply override it. No checks performed.

	StackAll. No checks performed. Everything is stacked as is.
*/
UENUM()
enum class EGAModifierEffectStacking : uint8
{
	CantStackSameType,
	SameTypeOverride,
	StackAll,
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