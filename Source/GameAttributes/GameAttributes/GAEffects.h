#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributes.h"
#include "GAEffects.generated.h"
/*
This struct contains information needed for effect, to modify attributes
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
	TODO::!!!!!!!!!!!
	Need way to cache this off, for two reasons.
	1. There is quite a lot of useful information in this spec, which can be reused later in active effects.
	2. So we don't have to copy this shit all over the place. And no. This is not even big yet.
	It still misses a lot of informations.
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
	TArray<FGAAttributeData> GetPeriodModifiers();
	TArray<FGAAttributeData> GetOnEndedModifiers();
	TArray<FGAAttributeData> GetOnRemovedModifiers();

	TArray<FGAAttributeData> EvalModifiers;
	TArray<FGAAttributeData> PeriodMods;
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

	TArray<FGAAttributeData> OnAppliedModifiers;
	TArray<FGAAttributeData> OnEndedModifiers;
	TArray<FGAAttributeData> OnRemovedModifiers;
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

public:
	virtual void ActivateEffect() override;
	virtual void FinishEffect() override;
	FGAActiveDuration()
	{}
	FGAActiveDuration(const FGAEffectContext& ContextIn)
		//:	Context(ContextIn)
	{
		Context = ContextIn;
	}
	FGAActiveDuration(const FGAEffectContext& ContextIn, FGAEffectSpec& SpecIn,
		const FGAEffectHandle& HandleIn);

	~FGAActiveDuration();
};

USTRUCT()
struct FGAActivePeriodic : public FGAActiveDuration
{
	GENERATED_USTRUCT_BODY()
public:
	void OnPeriod();
	void OnApplied();
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

	FGAEffectHandle HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

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


