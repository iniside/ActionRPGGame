#pragma once
#include "GameAttributes.h"
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

//tells execution function, which modifier should be executed from effect ?
enum class EGAModifierApplication : uint8
{
	OnApplied,
	OnPeriod,
	OnDuration,
	OnExpired,
	OnRemoved
};

/*
Type of modification applied to attribute:
Damage - Will damage attribute (ie, subtract value from Health)
Heal - will heal attribute (ie. add value to health)
*/
UENUM()
enum class EGAAttributeChangeType : uint8
{
	Damage,
	DamagePercentage,
	Heal,
	HealPercentage,
	Invalid
};

/*
	I seriosuly need to clean this shit up.
*/
/*
	TODO:: I probabaly need to change it to normal enum.
	So I can use it as array indexes.
*/
UENUM()
enum class EGAAttributeMod : uint8
{
	Add, //Value =  Value + X
	Subtract, //Value =  Value - X
	Multiply, //Value =  Value * X
	Divide,//Value =  Value * X - ok its's not really divide.
	Set, //Value = X
	PercentageAdd,
	PercentageSubtract,
	Invalid
};

UENUM()
enum class EGAAttributeSource : uint8
{
	Instigator,
	Target
};

UENUM()
enum class EGAModifierTarget : uint8
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
enum class EGAModifierType : uint8
{
	Outgoing,
	Incoming
};

UENUM()
enum class EGAEffectType : uint8
{
	Instant,
	Periodic,
	Duration,
	Infinite
};
/*

	StrongetOverride - Does not check for effect type/tags. It will just check if modified
	attribute, is already modified, and if incoming effect is stronger it will override
	all modifiers affecting this attribute, and remove all weaker ones.

	Override - does not check if it is stronger/weaker, it will simply override any existing modifiers
	and effects with the same name.

	Duration - Will add duration to existing effect of EXACTLY the same type.

	Intensity - Undefined. It will either add new effects to stack,
	or it will simply sum modifiers, from all effects of the same types, for the same attributes
	and refresh duration to the latest applied effect.

	Add - no checks, simply add new effect to stack.
*/
UENUM()
enum class EGAEffectStacking : uint8
{
	StrongerOverride, //only one effect modifing these attributes, will be present
	Override, //override existing effect of the same type.
	Duration, //will add duration to existing effect
	Intensity, //will add magnitude to existing effect.
	Add, //will simply add new effect

	Invalid
};

UENUM()
enum class EGAEffectModifierStacking : uint8
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

struct GAMEATTRIBUTES_API EnumToString
{
	static FString GetModifierDirectionAsString(EGAModifierDirection ModDir)
	{
		switch(ModDir)
		{
		case EGAModifierDirection::Incoming:
			return FString("Incoming");
		case EGAModifierDirection::Outgoing:
			return FString("Outgoing");
		}
		return FString("");
	}

	static FString GetStatckingAsString(EGAEffectStacking Stacking)
	{
		switch (Stacking)
		{
		case EGAEffectStacking::StrongerOverride:
			return FString("StrongerOverride");
		case EGAEffectStacking::Override:
			return FString("Override");
		case EGAEffectStacking::Intensity:
			return FString("Intensity");
		case EGAEffectStacking::Duration:
			return FString("Duration");
		case EGAEffectStacking::Add:
			return FString("Add");
		}
		return FString("");
	}
};

/*
	Special struct, which allows to use FGameplayTagContainer as key, for TSet and TMap.
	Bear in mind slower inserts/remove, but allow for complex keys.
*/
struct GAMEATTRIBUTES_API FGAHashedGameplayTagContainer
{
public:
	FGameplayTagContainer Tags;

private:
	FName Key;
	void GenerateFNameKey();

public:
	FGAHashedGameplayTagContainer()
	{};
	FGAHashedGameplayTagContainer(const FGameplayTagContainer& TagsIn);

	friend uint32 GetTypeHash(const FGAHashedGameplayTagContainer& InHandle)
	{
		return ::GetTypeHash(InHandle.Key);
	}
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectHandle
{
	GENERATED_USTRUCT_BODY()
protected:
	UPROPERTY() //we will replicate handle, so we need it be uproperty.
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

	inline int32 GetHandle() const { return Handle; }

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
struct GAMEATTRIBUTES_API FGAModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		EGAAttributeMod AttributeMod;
	UPROPERTY()
		float Value;

	EGAModifierType ModifierType;

	/* 
		Weak pointer to effect, which added this modifier.
		Useful because, while effect exist it have lots of useful informations.
	*/
	TWeakPtr<struct FGAActiveDuration> Effect;

		FGAModifier()
	{};
	FGAModifier(EGAAttributeMod ModIn, float ValueIn)
		: AttributeMod(ModIn),
		Value(ValueIn)
	{};
	FGAModifier(EGAAttributeMod ModIn, float ValueIn, TSharedPtr<struct FGAActiveDuration> ActiveEffect);
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

	inline FString ToString()
	{
		return AttributeName.ToString();
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
struct GAMEATTRIBUTES_API FGAEffectContext
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Just copy entire hit result struct.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FHitResult HitResult;
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

	inline bool IsValid() const
	{
		if (Target.IsValid() && Causer.IsValid() && Instigator.IsValid() && TargetComp.IsValid() && InstigatorComp.IsValid())
			return true;
		//UE_LOG(GameAttributesEffects, Error, TEXT("Effect Context Is Not Valid"));
		return false;
	}

	inline FString ToString()
	{
		if (!IsValid())
		{
			//UE_LOG(GameAttributesEffects, Error, TEXT("Effect Context Is Not Valid"));
			return FString("Context Is not valid");
		}
		FString ret;
		ret = "TargetHitLocation: ";
		ret += TargetHitLocation.ToString();
		ret += "\n";
		ret += "Target: ";
		ret += Target->GetName();
		ret += "\n";
		ret += "Causer: ";
		ret += Causer->GetName();
		ret += "\n";
		ret += "Instigator: ";
		ret += Instigator->GetName();
		ret += "\n";
		ret += "TargetComp: ";
		return ret;
	}

	

	void Reset();

	class UGAAttributesBase* GetTargetAttributes();
	class UGAAttributesBase* GetInstigatorAttributes();

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
struct GAMEATTRIBUTES_API FGACountedTagContainer
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

	inline FGameplayTagContainer GetTags() { return AllTags; };

	void AddTag(const FGameplayTag& TagIn);
	void AddTagContainer(const FGameplayTagContainer& TagsIn);
	void RemoveTag(const FGameplayTag& TagIn);
	void RemoveTagContainer(const FGameplayTagContainer& TagsIn);

	bool HasTag(const FGameplayTag& TagIn);
	bool HasAllTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);
	bool HasAnyTags(const FGameplayTagContainer& TagsIn, bool bCountEmptyAsMatch);

	inline int32 GetTagCount(const FGameplayTag& TagIn) const
	{
		return CountedTags.FindRef(TagIn);
	}
};


USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectCueParams
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Gameplay Cue")
		FHitResult HitResult;

	/** Instigator actor, the actor that owns the ability system component */
	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Cue")
		TWeakObjectPtr<AActor> Instigator;

	/** The physical actor that actually did the damage, can be a weapon or projectile */
	UPROPERTY(BlueprintReadWrite, Category = "Gameplay Cue")
		TWeakObjectPtr<AActor> EffectCauser;

	/* Tick interval for periodic effects */
	UPROPERTY(BlueprintReadOnly, Category = "GameplayCue")
		float Period;

	UPROPERTY(BlueprintReadOnly, Category = "GameplayCue")
		float Duration;

	FGAEffectCueParams()
	{};
	FGAEffectCueParams(const FHitResult& HitIn, AActor* InstigatorIn, AActor* EffectCauserIn)
		: HitResult(HitIn),
		Instigator(InstigatorIn),
		EffectCauser(EffectCauserIn)
	{};
	//bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};