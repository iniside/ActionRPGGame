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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
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
		TSubclassOf<class UGAEffectSpecification> EffectSpec;
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectName EffectName;
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
		TArray<FGAEffectModifier> EffectModifiers;
	
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAEffectDuration EffectDuration;

	/*
		Tags, which describe this effect. Please, refrain
		from using more than one tag! 
		Like Condition, Condition.Bleed, Boon, Enchatment,
		Hex, Status, whatever.
		If you use more than one tag, it might and will produce
		unpredictable results, which are going to be hard to balance out.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer EffectTags;
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
		Other effect must have these tags if I'm going to stack with it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer StackRequireTags;
	/*
		If effect I'm stacking with have these tags, I will remove it.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer StackingRemoveTags;

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

	void OnApplied() {};
	TArray<FGAAttributeData> OnAppliedModifiers;

	FGameplayTagContainer OwnedTags;
};
USTRUCT()
struct FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
		FGAEffectHandle MyHandle;
	UPROPERTY()
		FGAEffectContext Context;
	TArray<FGAAttributeData> OnAppliedModifiers;

	/*
		Tags I have accumulated from spec.
	*/
	FGameplayTagContainer OwnedTags;
	void OnApplied();
public:
	void ActivateEffect() {};
};

USTRUCT()
struct FGAActiveDuration : public FGAActiveBase
{
	GENERATED_USTRUCT_BODY()
public:
	void OnPeriod();
	void OnRemoved() {};
	void OnEnded();
protected:
	FTimerHandle PeriodTimerHandle;
	FTimerHandle DurationTimerHandle;
	
	TArray<FGEffectModifierGroup> EffectModifiers;

	TArray<FGAAttributeData> OnEndedModifiers;
	TArray<FGAAttributeData> OnRemovedModifiers;
	TArray<FGAAttributeData> PeriodModifiers;

	float Duration;
	float Period;

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
		OutgoingDivideMod(1),
		IncomingAddtiveMod(0),
		IncomingSubtractMod(0),
		IncomingMultiplyMod(0),
		IncomingDivideMod(1)
	{}
protected:
	/*
	Group array of modifiers, by handle to effect,
	so we know, which modifier were applied by which effect
	so we can easily remove them along effect.
	*/
	TMap<FGAEffectHandle, TArray<FGAEvalData>> EffectMods;
	//requiredTag, Mods
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
	void ApplyModifiers(FGAAttributeData& EvalData, const FGAEffectContext& Ctx);
	void AddModifierEffect(const FGAEffectHandle& HandleIn, const FGAEvalData& Eval);
	void RemoveModifierEffect(const FGAEffectHandle& HandleIn);

	int32 GetModifierCount();
	//recalculates entire stack, every time effect is added/or removed.
	void CalculateNewModifierStack();

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
/*
	Struct containg information about which effects, currently affect,
	this particular attribute tag.
*/
USTRUCT()
struct FGAActiveEffectModifier
{
	GENERATED_USTRUCT_BODY()
public:
	FGAActiveEffectModifier()
		: OutgoingAddtiveMod(0),
		OutgoingSubtractMod(0),
		OutgoingMultiplyMod(0),
		OutgoingDivideMod(1),
		IncomingAddtiveMod(0),
		IncomingSubtractMod(0),
		IncomingMultiplyMod(0),
		IncomingDivideMod(1)
	{}
protected:

	/*
		Group array of modifiers, by handle to effect,
		so we know, which modifier were applied by which effect
		so we can easily remove them along effect.
	*/
	TMap<FGAEffectHandle, TArray<FGAEvalData>> EffectMods;

	/*
		What type of effects, currently affect this stack.
	*/
	FGameplayTagContainer AffectingEffectsTags;

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
};
/*
	Contains array of effects, associated with this particular effect (by Handle);
*/
USTRUCT()
struct FGAEffectModifierHandles
{
	GENERATED_USTRUCT_BODY()
public:
	FGAEffectHandle Handle;
	TArray<FGAEffectModifier> Modifiers;
	void AddMod(const FGAEffectModifier& ModIn);
	FGAEffectModifierHandles()
	{}
	FGAEffectModifierHandles(const FGAEffectHandle& HandleIn, const TArray<FGAEffectModifier>& ModsIn)
		:	Handle(HandleIn),
			Modifiers(ModsIn)
	{}
};

USTRUCT()
struct FGAEffectModifiersCont
{
	GENERATED_USTRUCT_BODY()
public:
	TArray<FGAEffectModifierHandles> Modifiers;
};

USTRUCT()
struct FGAEffectModifiersContainer
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Treat is as map of transient attributes.
		These attributes are created at runtime, using specific tags.

		This should support:
		1. Stacking rules. Internally. Example
		Assume we have two tags Damage and Damage.Fire.
		Damage = +40%
		Damage.Fire = +30%
		Incoming attribute tag Damage.Fire
		Stacking rule: StrongestOverride.

		In this Damage, should take precedence since, it's highest value and
		most universal.
		Damage.Fire should be discarded.

		In case of:
		Damage = +20%
		Damage.Fire = +40%
		Incoming attribute tag Damage.Ice
		Stacking rule: StrongestOverride.
		Damage again will take preedence. Damage.Ice is not compatibile with Damage.Fire
		And highest available bonus is Damage.

		Assuming bonuses stack..

		Since each bonus lives within it's own tag, and it's not simple percentage
		(it's calculated magnitude or add, subtract, multiply, divide)
		we can't just linearlly go trough everything and add one after another.
		It will create unpredictable behaviour, which will entirely depend on order
		in which bonuses exist on actor.

		1. Stacking rules:
		a). Set global stacking rule, which will be simply applied to everything.
		b). Set stacking rule per tag, and aggregate bonuses based on it.
		
		If we have global rule we:
		1. Find highest applicable value, modify by it and go on.
		2. Find all applicable values, sum them, then apply, and go on.

		If we have rule per attribute:
		1. Find applicable tags. 
		2. Check their stacking rules.
		3. Decide which rule have percedence.
		4. If one tag is HighestOverride and there are two other mods with Stack Intensity we:
		a). Stack two latter effects to calculate their magnitude.
		b). Compare to HighestOverride.
		c). Take Highest from two results.
		Kind of crap. I probabaly should not do this.

		Another option.
		Stacking handled per modifier type. Modifier type is tag. Damage, Damge.Fire. Etc.
		1. We check if it can stack before we apply this.
		2. Then internally we use one global rule. Either stack or don't stack, 
		depending on hierarchy.
		3. How to handle stacking between different Effect Types ?
		Effect Type is different, we want to set rules
		if this effect can stack with other effects, which improve the same AttributeTag
		If we have two effect which improve Damage.Fire one of type Boon and one of type
		Enchantment, how do we decide if they can stack ?
		Add tag container indicated, that it can stack with ?
	*/
public:
	/*
		Damage
		Damage.Fire
		Problem.
	*/
	TMap<FGameplayTag, FGAEffectModifiersCont> Modifiers;

	TMap<FGAEffectHandle, TArray<FGAEffectModifier>> AllModifiers;
public:
	void AddMods(const FGAEffectHandle& HandleIn, const TArray<FGAEffectModifier>& ModsIn);

	void RemoveMods(const FGAEffectHandle& HandleIn);
	void RemoveMod(const FGAEffectHandle& HandleIn, FGameplayTagContainer& ModTags);
	void ApplyModifiersToEffect(FGAAttributeData& EffectIn, const FGameplayTagContainer& EffectTags
		,const FGAEffectContext& Ctx);

	void StackModifiers(FGAEffectSpec& SpecIn);
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

	/*
		Execute modifiers from existing effect, spec on incoming effect.
	*/
	void ExecuteEffectModifier(FGAAttributeData& ModifierIn, 
		const FGameplayTagContainer& EffectTags, const FGAEffectContext& Ctx);

	FGAEffectHandle HandleInstantEffect(FGAEffectInstant& SpecIn, const FGAEffectContext& Ctx);

	//merge duration and periodic.
	FGAEffectHandle HandleDurationEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

	
	FGAEffectHandle AddActiveEffect(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);

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
	FGAEffectHandle CheckTargetEffectOverride(FGAEffectSpec& EffectIn, const FGAEffectContext& Ctx);
	
	void Clean();
	FGAEffectModifiersContainer EffectMods;
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


