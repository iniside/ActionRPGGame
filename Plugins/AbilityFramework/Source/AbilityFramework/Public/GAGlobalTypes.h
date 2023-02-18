#pragma once
#include "AbilityFramework.h"
#include "GameplayTagsModule.h"
#include "GameplayTagContainer.h"
//#include "Messaging.h"
#include "GAGlobalTypes.generated.h"

void AddLogDebugInfo(FString Text, UWorld* World);

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
/*
	TODO:: I probabaly need to change it to normal enum.
	So I can use it as array indexes.
*/
UENUM()
enum class EGAAttributeMod : int32
{
	Add = 0, //Value =  Value + X
	Subtract = 1, //Value =  Value - X
	Multiply = 2, //Value =  Value * X
	Divide = 3,//Value =  Value * X - ok its's not really divide.
	Set = 4, //Value = X
	PercentageAdd = 5,
	PercentageSubtract = 6,
	Invalid = 7
};

UENUM()
enum class EGAAttributeSource : uint8
{
	Instigator,
	Target, 
	Causer,
	Ability
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
enum class EGAEffectType : uint8
{
	Instant = 0,
	Duration = 1,
	Infinite = 2,
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
	Add = 0,
	Duration = 1,
	Override = 2, //override existing effect of the same type, and all other effects which have override the same attribute and have same AttributeTags
	Intensity = 3,
	 //will add duration to existing effect
	 //will simply add new effect
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

USTRUCT()
struct FAFAtributeRowData : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float BaseValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MinValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float MaxValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CurrentValue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<class UGAAttributeExtension> Extension;
};


USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectContext
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

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributesBase> TargetAttributes;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UGAAttributesBase> InstigatorAttributes;

	/**
	*	Direct Reference to TargetActor (I will possibly remove FHitResult Target!
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<UObject> Target;
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

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<AActor> Avatar;
	/**
	*	Attribute component of Target.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UAFAbilityComponent> TargetComp;
	/**
	*	Attribute component of Intigator
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TWeakObjectPtr<class UAFAbilityComponent> InstigatorComp;

	class IAFAbilityInterface* TargetInterface;
	class IAFAbilityInterface* InstigatorInterface;

	void SetTarget(UObject* NewTarget, const FHitResult& InHit);
	template<class T>
	inline T* GetTarget()
	{
		return Cast<T>(Target);
	}

	inline bool IsValid() const
	{
		/*if (Target.IsValid() && Causer.IsValid() && Instigator.IsValid() && TargetComp.IsValid() && InstigatorComp.IsValid())
		return true;*/
		if (Causer.IsValid() && Instigator.IsValid() && InstigatorComp.IsValid() && TargetInterface)
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

	bool operator==(UObject* Other) const
	{
		return FObjectKey(Target.Get()) == FObjectKey(Other);
	}

	bool operator==(const FObjectKey& Other) const
	{
		return FObjectKey(Target.Get()) == Other;
	}

	void Reset();

	class UGAAttributesBase* GetTargetAttributes();
	class UGAAttributesBase* GetInstigatorAttributes();
	class UGAAttributesBase* GetCauserAttributes();

	class UGAAttributesBase* GetTargetAttributes() const;
	class UGAAttributesBase* GetInstigatorAttributes() const;
	class UGAAttributesBase* GetCauserAttributes() const;

	class UAFEffectsComponent* GetTargetEffectsComponent();
	class UAFEffectsComponent* GetTargetEffectsComponent() const;
	void operator=(const FGAEffectContext& Other);
	FGAEffectContext()
	{}

	FGAEffectContext(const FGAEffectContext& Other);

	FGAEffectContext(APawn* InInstigator, UObject* InCauser);

	FGAEffectContext(TWeakObjectPtr<class UGAAttributesBase> TargetAttributesIn, TWeakObjectPtr<class UGAAttributesBase> InstigatorAttributesIn,
		const FVector& TargetHitLocationIn, TWeakObjectPtr<UObject> TargetIn,
		TWeakObjectPtr<UObject> CauserIn, TWeakObjectPtr<APawn> InstigatorIn,
		TWeakObjectPtr<class UAFAbilityComponent> TargetCompIn,
		TWeakObjectPtr<class UAFAbilityComponent> InstigatorCompIn,
		TWeakObjectPtr<class AActor> InAvatar);

	~FGAEffectContext();
};


struct FGAEffect;
class UGAGameEffectSpec;
struct FGAEffectMod;
struct FGAAttribute;

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectHandle
{
	GENERATED_BODY()
protected:
	//just to be safe we don't run out of numbers..
	UPROPERTY(VisibleAnywhere, Transient)
		int32 Handle; //Fname Guid ?
public:
	int32 GetHandle() const
	{
		return Handle;
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

	//FGAEffectHandle& operator=(const FGAEffectHandle& Other)
	//{
	//	Handle = Other.Handle;
	//	EffectPtr = Other.EffectPtr;
	//	return *this;
	//}

	void Reset();
	bool IsValid() const;
	friend uint32 GetTypeHash(const FGAEffectHandle& InHandle)
	{
		return InHandle.Handle;
	}

	FGAEffectHandle();

	FGAEffectHandle(uint32 HandleIn);
	void PostScriptConstruct();
public:
	~FGAEffectHandle();
};
//
template<>
struct TStructOpsTypeTraits< FGAEffectHandle > : public TStructOpsTypeTraitsBase2<FGAEffectHandle>
{
	enum
	{
		WithPostScriptConstruct = true,
	};
};

USTRUCT()
struct ABILITYFRAMEWORK_API FAFPredictionHandle
{
	GENERATED_BODY()
public:
	//ID of current handle.
	UPROPERTY()
		uint32 Handle;
	UPROPERTY()
		FGAEffectHandle EffectHandle;

	uint64 Timestamp;

	static FAFPredictionHandle GenerateClientHandle(UAFAbilityComponent* InComponent);
	/*
		Was prediction successful ?
		If true nothing happens on client (might interpolate to result from server).
		If false, server will override client predicted results.
	*/
	UPROPERTY()
		bool bPredictionValid;


	bool IsValid() const
	{
		return true;
	}

	const bool operator==(const FAFPredictionHandle& Other) const
	{
		return Handle == Other.Handle;
	}
	friend uint32 GetTypeHash(const FAFPredictionHandle& InHandle)
	{
		return InHandle.Handle;
	}
};

DECLARE_MULTICAST_DELEGATE(FGAGenericDelegate);

struct ABILITYFRAMEWORK_API EnumToString
{
	static FString GetStatckingAsString(EGAEffectStacking Stacking)
	{
		switch (Stacking)
		{
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
struct ABILITYFRAMEWORK_API FGAHashedGameplayTagContainer
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
struct ABILITYFRAMEWORK_API FGAIndividualMods
{
	GENERATED_BODY()
public:
	UPROPERTY()
		float Additive;
	UPROPERTY()
		float Subtractive;
	UPROPERTY()
		float Multiplicative;
	UPROPERTY()
		float Divide;
	UPROPERTY()
		float PercentageAdd;
	UPROPERTY()
		float PercentageSubtract;

	FGAIndividualMods()
		: Additive(0.0f),
		Subtractive(0.0f),
		Multiplicative(0.0f),
		Divide(0.0f),
		PercentageAdd(0.0f),
		PercentageSubtract(0.0f)
	{};
	FGAIndividualMods(float AdditiveIn,
		float SubtractiveIn,
		float MultiplicativeIn,
		float DivideIn,
		float PercentageAddIn,
		float PercentageSubtractIn
	)
		: Additive(AdditiveIn),
		Subtractive(SubtractiveIn),
		Multiplicative(MultiplicativeIn),
		Divide(DivideIn),
		PercentageAdd(PercentageAddIn),
		PercentageSubtract(PercentageSubtractIn)
	{}
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
struct ABILITYFRAMEWORK_API FGAAttribute
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
	friend uint32 GetTypeHash(const FGAAttribute& InAttribute)
	{
		return GetTypeHash(InAttribute.AttributeName);
	}
	//FGAAttribute(const FString& AttributeNameIn)
	//{
	//	AttributeName = *AttributeNameIn;
	//};
};

/* Final calculcated mod from effect, which can be modified by Calculation object. */
USTRUCT()
struct ABILITYFRAMEWORK_API FGAEffectMod
{
	GENERATED_BODY()
public:
	FGAAttribute Attribute;
	UPROPERTY()
		float Value;
	EGAAttributeMod AttributeMod;
	struct FGAEffectHandle Handle;
	FGameplayTagContainer AttributeTags;
	/*
	Spec from which this mod has been derived.
	Used to do not copy to much heavy data around.
	*/
	inline bool CompareMods(const FGAEffectMod& OtherMod) const
	{
		return AttributeMod == OtherMod.AttributeMod;
	}
	inline bool HasAllTags(const FGameplayTagContainer& TagsIn) const
	{
		return AttributeTags.HasAll(TagsIn);
	}
	inline bool HasAllTagsExact(const FGAEffectMod& OtherMod) const
	{
		return AttributeTags.HasAllExact(OtherMod.AttributeTags);
	}
	inline bool HasAllTagsExact(const FGameplayTagContainer& TagsIn) const
	{
		return AttributeTags.HasAllExact(TagsIn);
	}
	inline bool HasAllTagsIncludingChildren(const FGameplayTagContainer& TagsIn) const
	{
		return TagsIn.HasAll(AttributeTags);
	}
	const bool operator==(const FGAEffectMod& Other) const
	{
		return Value == Other.Value && AttributeMod == Other.AttributeMod;
	}
	const bool operator!=(const FGAEffectMod& Other) const
	{
		return Value != Other.Value && AttributeMod == Other.AttributeMod;
	}

	const bool operator>(const FGAEffectMod& Other) const
	{
		return Value > Other.Value && AttributeMod == Other.AttributeMod;
	}

	const bool operator<(const FGAEffectMod& Other) const
	{
		return Value < Other.Value && AttributeMod == Other.AttributeMod;
	}
	const bool operator>=(const FGAEffectMod& Other) const
	{
		return Value >= Other.Value && AttributeMod == Other.AttributeMod;
	}

	const bool operator<=(const FGAEffectMod& Other) const
	{
		return Value <= Other.Value && AttributeMod == Other.AttributeMod;
	}

	
	FGAEffectMod()
		: Attribute(NAME_None),
		Value(0),
		AttributeMod(EGAAttributeMod::Invalid)
	{};

	FGAEffectMod(const FGAAttribute& AttributeIn, float ValueIn,
		EGAAttributeMod AttributeModIn, FGAEffectHandle HandleIn, FGameplayTagContainer AttributeTagsIn)
		: Attribute(AttributeIn),
		Value(ValueIn),
		AttributeMod(AttributeModIn),
		Handle(HandleIn),
		AttributeTags(AttributeTagsIn)
	{
	};
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FAFAttributeChangedData
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FGAEffectMod Mod;

	UPROPERTY()
		TWeakObjectPtr<UObject> Target;
	UPROPERTY()
		FVector Location;

	UPROPERTY()
		float NewValue;
};

/*
Struct representing final modifier applied to attribute.
*/
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		EGAAttributeMod AttributeMod;
	UPROPERTY()
		float Value;

	FGameplayTagContainer Tags;
	/* 
		Weak pointer to effect, which added this modifier.
		Useful because, while effect exist it have lots of useful informations.
	*/
	struct FGAEffectHandle Handle;

		FGAModifier()
	{};
	FGAModifier(EGAAttributeMod ModIn, float ValueIn)
		: AttributeMod(ModIn),
		Value(ValueIn)
	{};
	FGAModifier(EGAAttributeMod ModIn, float ValueIn, FGAEffectHandle HandleIn)
		: AttributeMod(ModIn),
		Value(ValueIn),
		Handle(HandleIn)
	{};

	FGAModifier(const FGAEffectMod& ModIn)
		: AttributeMod(ModIn.AttributeMod),
		Value(ModIn.Value),
		Handle(ModIn.Handle)
	{};
};

USTRUCT()
struct ABILITYFRAMEWORK_API FGACountedTagContainer
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
public:
	UPROPERTY()
		FGameplayTagContainer AllTags;
public:

	inline FGameplayTagContainer GetTags() { return AllTags; };

	void AddTag(const FGameplayTag& TagIn);
	void AddTagContainer(const FGameplayTagContainer& TagsIn);
	void RemoveTag(const FGameplayTag& TagIn);
	void RemoveTagContainer(const FGameplayTagContainer& TagsIn);

	bool HasTag(const FGameplayTag& TagIn);
	bool HasTagExact(const FGameplayTag TagIn);
	bool HasAny(const FGameplayTagContainer& TagsIn);
	bool HasAnyExact(const FGameplayTagContainer& TagsIn);
	bool HasAll(const FGameplayTagContainer& TagsIn);
	bool HasAllExact(const FGameplayTagContainer& TagsIn);
	
	bool HasTag(const FGameplayTag& TagIn) const;
	bool HasTagExact(const FGameplayTag TagIn) const;
	bool HasAny(const FGameplayTagContainer& TagsIn) const;
	bool HasAnyExact(const FGameplayTagContainer& TagsIn) const;
	bool HasAll(const FGameplayTagContainer& TagsIn) const;
	bool HasAllExact(const FGameplayTagContainer& TagsIn) const;

	inline FGameplayTagContainer& GetAllTags()
	{
		return AllTags;
	}

	inline int32 GetTagCount(const FGameplayTag& TagIn) const
	{
		return CountedTags.FindRef(TagIn);
	}
};


USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectCueParams
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
		TWeakObjectPtr<UObject> Causer;

	UPROPERTY(BlueprintReadOnly)
		FGameplayTagContainer CueTags;

	FGAEffectCueParams()
	{};
	FGAEffectCueParams(const FHitResult& InHitResult, AActor* InstigatorIn, UObject* CauserIn)
		: HitResult(InHitResult),
		Instigator(InstigatorIn),
		Causer(CauserIn)
	{};
	FGAEffectCueParams(const FGAEffectContext& InContext, const struct FGAEffectProperty& InProperty);
	//bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
};

USTRUCT()
struct ABILITYFRAMEWORK_API FAFCueHandle
{
	GENERATED_BODY();
private:
	UPROPERTY()
		uint32 Handle;

	FAFCueHandle(uint32 InHandle)
		: Handle(InHandle)
	{}
public:
	FAFCueHandle()
		: Handle(0)
	{}

	static FAFCueHandle GenerateHandle();

	bool operator==(const FAFCueHandle Other) const
	{
		return Handle == Other.Handle;
	}

	friend uint32 GetTypeHash(const FAFCueHandle& InHandle)
	{
		return InHandle.Handle;
	}
};
