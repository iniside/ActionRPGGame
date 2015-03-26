#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
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
		Attribute other effect modify, which we will modify on this effect.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	/*
		Only one tag, per one modifier, so we have EXACT match.
		If you want more generic modifier, you just need to use Tag higher in hierarchy.

		If empty, modify everything.

		DEPRECATED
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTag AttributeTag;
	/*
		How this modifier should stack.
	*/
	UPROPERTY(EditAnywhere)
		EGAEffectStacking Stacking;
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
		I require these tags on effect I'm going to modify.
		If empty, I will modify all effects. Which have specified attribute.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;
	/*
		I can stack only with effects, which have these tags and modify the same
		attribute as me.
		If empty I will stack with any effect which modify the same attribute as me.

		This means if stacking rule is set to Highest Override, and we have
		two effects affecting Damage.Fire One effect is of type Boon, other of type Enchantment.

		in Boon::RequiaredStackingTags - Enchantment.
		In Enchantment::RequiaredStackingTags - Boon.

		Now we already have enchatnment. We apply Boon. Since their tags match, they
		are going to be stacked.

		Now assume we have Enchantment, and apply another enchantment. 
		Stronger one will override weak, since they do not have matching tags.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredStackingTags;
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
	Struct, which represent attribute modified by effect.
*/
//USTRUCT(BlueprintType)
//struct FGAModifiedAttribute
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	UPROPERTY()
//		FGAAttribute Attribute;
//};

USTRUCT(BlueprintType)
struct FGAEffectDuration
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	-1 infinite duration, 0-no duration >0 - set duration
	*/
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Duration;
	/*
	<=0 - no period >0 - set period
	*/
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Period;
};
/*
Encapsulates effect name into struct.
We will use to uniqely identify effects.

Idea is simple:
1. Generic effects, will have generic names (like bleed, bunrining etc).
2. Specific effects, will derive name from ability/weapon/whatever they are applied from.
*/
USTRUCT(BlueprintType)
struct FGAEffectName
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FName EffectName;

	FGAEffectName()
		: EffectName(NAME_None)
	{}
	FGAEffectName(const FName& EffectNameIn)
		: EffectName(EffectNameIn)
	{}

	const bool operator==(const FGAEffectName& OtherIn) const
	{
		return OtherIn.EffectName == EffectName;
	}

	friend uint32 GetTypeHash(const FGAEffectName& EffectNameIn)
	{
		return ::GetTypeHash(EffectNameIn.EffectName);
	}
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

USTRUCT(BlueprintType)
struct FGAAttributeEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Modifier for attributes, when effect is initially applied.

		Instant application effects, do use only this property.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier InitialAttribute;
	/*
		Modifier which is applied for effect duration.
		It's removed when effect is removed/expire.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier DurationAttribute;
	/*
		Modifier applied when effect Ticks (on period interval).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier PeriodAttribute;
	/*
		Modifier applied when effect is removed from target.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier RemovedAttribute;
	/*
		Modifier applied when Effect naturally expire.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier ExpiredAttribute;
};

/*
	Base struct containing information about effect. It is not applied directly to targets.
*/
USTRUCT(BlueprintType)
struct FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Name of the effect.

		Name of effect is derived from object (Causer) which apply this effect, be it ability, character,
		projectile, weapon etc.

		If you have blueprint ability called Fireball, name of effect will be Fireball_C.

		It's used internally to track effects, from different abilities, as effect on it's own, does
		not have any real meaning.
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectName EffectName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectPolicy Policy;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		FGAEffectDuration EffectDuration;

	/*
		Context of effect containing info about target, instigator, causer etc.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Base")
		FGAEffectContext Context;
	/*
		Spec Containing attributes, which this effect will modify.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeEffectSpec AttributeSpec;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier InitialAttribute;
	/*
		Modifier which is applied for effect duration.
		It's removed when effect is removed/expire.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier DurationAttribute;
	/*
		Modifier applied when effect Ticks (on period interval).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier PeriodAttribute;
	/*
		Modifier applied when effect is removed from target.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier RemovedAttribute;
	/*
		Modifier applied when Effect naturally expire.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attributes")
		FGAAttributeModifier ExpiredAttribute;


	TArray<FGAAttributeData> EvalModifiers;
	TArray<FGAAttributeData> PeriodMods;


	FGAAttributeData GetInitialAttribute();
	FGAAttributeData GetDurationAttribute();
	FGAAttributeData GetPeriodAttribute();
	FGAAttributeData GetRemovedAttribute();
	FGAAttributeData GetExpiredAttribute();
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

/*
	What I want to achieve:
	1. Effect modifing the same attribute within the same depth, never stack.
	Ie. Damage + Damage don't stack. But Damage + Damage.Fire does.
	2. Only highest modifier for attribute can be active.
*/
/*
1. If spec is instant application, we will try to apply it directly without any fuss.
2. If it have duration (or is infinite), we will create FGAActiveEffect out of it.
	TODO::!!!!!!!!!!!
	Need way to cache this off, for two reasons.
	1. There is quite a lot of useful information in this spec, which can be reused later in active effects.
	2. So we don't have to copy this shit all over the place. And no. This is not even big yet.
	It still misses a lot of informations.
*/
//USTRUCT(BlueprintType)
//struct FGAEffectSpec
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	UPROPERTY()
//		TSubclassOf<class UGAEffectSpecification> EffectSpec;
//	/*
//		Name of the effect. 
//
//		Name of effect is derived from object (Causer) which apply this effect, be it ability, character,
//		projectile, weapon etc.
//
//		If you have blueprint ability called Fireball, name of effect will be Fireball_C.
//
//		It's used internally to track effects, from different abilities, as effect on it's own, does
//		not have any real meaning.
//	*/
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attribute Modifiers")
//		FGAEffectName EffectName;
//	/*
//		These properties will mirror the ones in GAEffectSpecification.
//		We use GAEffectSpecification as template, which is used to initialize, those properties
//		and then we can override them using various static functions.
//
//		Or we can just apply them directly.
//
//		EditAnywhere, so we can create an inline spec, when we need it.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		FGAEffectPolicy Policy;
//	/*
//		Modifiers applied to attribute for duration of effect.
//		Should only be really used with complex Attributes!
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAAttributeModifier> AttributeModifiers;
//	/*
//		Modifiers, to attributes applied when this effect applied to target.
//
//		Should only be used with float attributes!
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAAttributeModifier> AppliedModifiers;
//	/*
//		Modifiers which will be applied on effect period.
//
//		Should only be used with float attributes!
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAAttributeModifier> PeriodModifiers;
//	/*
//		Modifiers which will be applied when effect is prematurly removed.
//
//		Should only be used with float attributes!
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAAttributeModifier> RemovedModifiers;
//	/*
//		Modifiers which will be applied when effect naturally expired.
//
//		Should only be used with float attributes!
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAAttributeModifier> ExpiredModifiers;
//
//	/*
//		If I have duration, I will modify other effects, attribute modifiers,
//		if those effects meet these requirements.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		TArray<FGAEffectModifier> EffectModifiers;
//	/*
//		Duration of effect. Separate for Period duration, and total duration.
//
//		Right now you type, values directly.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
//		FGAEffectDuration EffectDuration;
//
//	/*
//		Tags, which describe this effect. 
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer EffectTags;
//	/*
//		I require these tags on target to be applied.
//		If this is empty I will ignore it.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer RequiredTags;
//	/*
//		I will apply these tags, to target if I'm applied.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer GrantedTags;
//	/*
//		I will add these immunity tags, to target if I'm applied.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer GrantedImmunityTags;
//	/*
//		Other effect must have these tags if I'm going to stack with it.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer StackRequireTags;
//	/*
//		If effect I'm stacking with have these tags, I will remove it.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer StackingRemoveTags;
//
//	/*
//		I require any of these tags, on other effect
//		to be able to modify it.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer OtherEffectRequire;
//
//
//	/*
//	I will do something when I expire, If target have these tags.
//
//	Ok. It's kind of dumb, I should do it differently, we need more data
//	about who is target (target doesn't need to be actor, to which
//	effect is applied, but for example, actor, who hit actor with this effect.
//	*/
//	UPROPERTY(EditAnywhere, Category = "Tags")
//		FGameplayTagContainer ExpiredRequireTags;
//
//	UPROPERTY()
//		FGAEffectContext Context;
//
//	FGAEffectSpec() {};
//	FGAEffectSpec(const FGAEffectContext& ContextIn)
//		: Context(ContextIn)
//	{};
//
//	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn, const FGAEffectContext& ContextIn);
//	/*
//	These constructors are used with conjuction with static functions,
//	to easily alter parts of effect spec, without spawning/chagning
//	UGAEffectSpecification, which can provide some defaults.
//	*/
//	/*
//	Constructor which will override duration.
//	*/
//	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
//		FGAEffectDuration DurationIn, const FGAEffectContext& ContextIn);
//	/*
//	Constructor which will override entire stack of modifiers.
//	*/
//	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
//		TArray<FGAAttributeModifier> ModifiersIn, const FGAEffectContext& ContextIn);
//	/*
//	Constructor which will override only specific modifiers.
//	*/
//	FGAEffectSpec(TSubclassOf<class UGAEffectSpecification> SpecIn,
//		TArray<FGAModifierOverride> OverridesIn, const FGAEffectContext& ContextIn);
//
//	TArray<FGAAttributeData> GetModifiers();
//	TArray<FGAAttributeData> GetAttributeModifiers();
//	TArray<FGAAttributeData> GetPeriodModifiers();
//	TArray<FGAAttributeData> GetOnEndedModifiers();
//	TArray<FGAAttributeData> GetOnRemovedModifiers();
//
//	TArray<FGAAttributeData> EvalModifiers;
//	TArray<FGAAttributeData> PeriodMods;
//};

USTRUCT()
struct FGAEffectInstant
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectInstant()
	{}
	FGAEffectInstant(FGAEffectSpec& SpecIn, const FGAEffectContext& ContextIn);

	void OnApplied() {};
	FGAAttributeData InitialAttribute;

	FGameplayTagContainer OwnedTags;
};
USTRUCT()
struct FGAActiveBase
{
	GENERATED_USTRUCT_BODY()

};

USTRUCT()
struct FGAActiveDuration : public FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
		friend struct FGAActiveEffectContainer;
	/* Current handle of this effect. */
	FGAEffectHandle MyHandle;

	/* Name of effect derived from EffectSpec. */
	FGAEffectName EffectName;

	/* Aggregation type derived from EffectSpec. */
	EGAEffectAggregation AggregationType;

	UPROPERTY()
		FGAEffectContext Context;

	/* Attribute applied initially by this effect. */
	FGAAttributeData InitialAttribute;
	/* 
		Attribute change applied for this effect duration. This is only really useful for 
		Complex Attributes.
	*/
	FGAAttributeData DurationAttribute;
	/* Attribute changes applied on each period. */
	FGAAttributeData PeriodModifiers;
	/* Attribute changes applied when effect is removed externally. */
	FGAAttributeData RemovedAttribute;
	/* Attribute changes applied when effect naturally expires. */
	FGAAttributeData ExpiredAttribute;
	/* Duration of effect. */
	float Duration;
	/* Time interval between periods. */
	float Period;

	//add captured attributes from Instigator/Source.
	/*
	Normally attributes on each tick, are checked from instigator, but when Instigator dies (is destroyed)
	we should revert to checking cached relevelant attributes directly in effect.
	*/

	/*
		Tags I have accumulated from spec.
	*/
	FGameplayTagContainer OwnedTags;
	

	void RemoveDurationAttribute();
		
public:
	/* 
		Called when effect is applied. 
		Applies InitialAttribute and DurationAttribute.
	*/
	void OnApplied();
	/* Called on period of this effect. */
	void OnPeriod();
	/* Called when effect externally removed. */
	void OnRemoved();
	/* Called when effect naturally expires. */
	void OnEnded();
protected:
	FTimerHandle PeriodTimerHandle;
	FTimerHandle DurationTimerHandle;

public:
	/*
		Return true if OtherIn is higher than, any existing mod.
	*/
	bool ComparePeriodModifiers(const FGAAttributeData& OtherIn);
	void RestartTimer(float NewDuration);
	void ActivateEffect();
	void FinishEffect();
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
struct FGAActiveInfinite : public FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
public:
};

USTRUCT()
struct FGAEffectTagHandle
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectName EffectName;
	FGAEffectHandle Handle;

	FGAEffectTagHandle()
	{}
	FGAEffectTagHandle(const FGAEffectName& EffectNameIn,
		const FGAEffectHandle& HandleIn)
		: EffectName(EffectNameIn),
		Handle(HandleIn)
	{};

	inline const bool operator==(const FGAEffectTagHandle& OtherIn) const
	{
		return OtherIn.EffectName == EffectName
			&& OtherIn.Handle == Handle;
	};
};

/*
	Group effects, per Instigator.
*/
USTRUCT()
struct FGAInstigatorEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		We can assume that effects, with the same tag, are of the same type.
	*/
	TMap<FGAEffectName, TArray<FGAEffectHandle>> EffectsByName;
	TArray<FGAEffectTagHandle> Effects;
};

/*
	Groups effects, by Target.
*/
USTRUCT()
struct FGATargetEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		We can assume that effects, with the same tag, are of the same type.
	*/
	TMap<FGameplayTag, TArray<FGAEffectHandle>> EffectsByTag;
};

USTRUCT()
struct FGAActiveEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	FGAActiveEffectContainer()
	{}
	

	/*
		Both functions are only for internal use!
		These are not safe to use outside of EffectContainer.
	*/
	/*
		Generic function, which will remove effect, given handle.
		Does not perform any checks.
	*/
	void RemoveActiveEffect(const FGAEffectHandle& HandleIn);
protected:
	void RemoveTargetAggregation(TSharedPtr<FGAActiveDuration> EffectIn);
	void RemoveInstigatorAggregation(TSharedPtr<FGAActiveDuration> EffectIn);
	/*
		Generic function, which will add any effect.

		It does not perform any checks.
	*/
	FGAEffectHandle AddActiveEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
public:
	FGAEffectHandle ApplyEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Ctx);
	void Clean();
	/*
		Execute modifiers from existing effect, spec on incoming effect.
	*/
	void ExecuteEffectModifier(FGAAttributeData& ModifierIn, 
		const FGameplayTagContainer& EffectTags, const FGAEffectContext& Ctx);
protected:
	FGAEffectHandle HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx);

	//merge duration and periodic.
	FGAEffectHandle HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle HandleInstigatorAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle	HandleInstigatorEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle HandleInstigatorEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle	HandleInstigatorEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/*
		If existing effect has been found, it will simply return handle to existing effect.
		If new effect has been added return handle to new effect.
	*/
	FGAEffectHandle	HandleInstigatorEffectDuration(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle HandleTargetAggregationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle	HandleTargetEffectStrongerOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	/*
		Simply find existing effect of the same type on target.
		Remove it.
		Apply new one.

		Does not check for anything.
	*/
	FGAEffectHandle HandleTargetEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	FGAEffectHandle HandleTargetEffectAdd(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	
	/*
		Map of all Active effects.
		Store as shared pointers, so we can have some basic polymorphism.

		Not, extremly important and might change later!
	*/
	TMap<FGAEffectHandle, TSharedPtr<FGAActiveDuration>> ActiveEffects;

	/*
		Group effects, by instigator who applied to it.
		Each instigator get separate pool, against which stacking rules are checked.
	*/
	TMap<TWeakObjectPtr<class UGAAttributeComponent>, FGAInstigatorEffectContainer> InstigatorEffects;

	/*
		Effects aggregated by Target.
	*/
	TMap<FGAEffectName, TArray<FGAEffectHandle>> MyEffects;

	/*
		Use it to store modifier tags (tags, from effects which modify other effect),
	*/
	FGACountedTagContainer ActiveModifierTags;

};


