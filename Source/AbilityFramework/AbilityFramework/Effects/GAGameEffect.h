#pragma once
//#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.h"
#include "GameplayTagContainer.h"
//#include "NetSerialization.h"
#include "GAGameEffect.generated.h"

DECLARE_STATS_GROUP(TEXT("GameEffect"), STATGROUP_GameEffect, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("GatherModifiers"), STAT_GatherModifiers, STATGROUP_GameEffect, );

/*
	Modifier type for simple attribute operatinos.
	It's only additive or subtractive due to possible race conditions with multiplicative/divide 
	mods.
*/
UENUM()
enum class EGASimpleAttributeOperation : uint8
{
	Additive,
	Subtractive,
	Invalid
};

UENUM()
enum class EGAMakeSpecResult : uint8
{
	NewHandle,
	OldHandle,
	Invalid
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAMagnitude
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	Type of calculation we want to perform for this Magnitude.
	*/
	UPROPERTY(EditAnywhere)
		EGAMagnitudeCalculation CalculationType;

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

	float GetFloatValue(const FGAEffectContext& Context);
};
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		EGAAttributeMod AttributeMod;

	/*
		Who will be target of this attribute change.
		Instigator - pawn which applied effect (regardless of to whom).
		Target - targeted pawn (regardless of who applied).
	*/
	UPROPERTY(EditAnywhere)
		EGAModifierTarget ModifierTarget;
	/*
		How modifier will be executed on target.
	*/
	//UPROPERTY(EditAnywhere, Category = "Execution Type")
	//	TSubclassOf<class UGAEffectExecution> ExecutionType;

	/*
		Modifier value applied to attribute
	*/
	UPROPERTY(EditAnywhere)
		FGAMagnitude Magnitude;
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FAFAttributeModifierContainer
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<FGAAttributeModifier> Modifiers;
};

UCLASS(Blueprintable, BlueprintType, EditInLineNew)
class ABILITYFRAMEWORK_API UGAGameEffectSpec : public UObject
{
	GENERATED_BODY()
public:
	/*
		Individual Tag descrbing effect type.
		ie. Condition.Burning
	*/
	UPROPERTY(EditAnywhere, Category = "Effect Info")
		FGameplayTag EffectTag;

	UPROPERTY(EditAnywhere, Category = "Effect Info")
		TSubclassOf<class UAFEffectApplicationRequirement> ApplicationRequirement;
	/* 
		How effect should stack. Only relevelant for periodic effects
		and modifiers applied on period.
	*/
	UPROPERTY(EditAnywhere, Category = "Stacking")
		TSubclassOf<class UAFEffectCustomApplication> Application;

	UPROPERTY(EditAnywhere, Category = "Stacking")
		int32 MaxStacks;
	
	/*
		Who should aggregate this effect. Relevelant for stacking type
		and by this only relevelant for periodic effects.
	*/
	UPROPERTY(EditAnywhere, Category = "Stacking")
		EGAEffectAggregation EffectAggregation;

	/* Total duration of effect (if applicable) */
	UPROPERTY(EditAnywhere, Category = "Duration")
		FGAMagnitude Duration;
	/* Total duration of effect (if applicable) */
	UPROPERTY(EditAnywhere, Category = "Duration")
		float MaxStackedDuration;
	/* Duration of single period. */
	UPROPERTY(EditAnywhere, Category = "Duration")
		FGAMagnitude Period;
	/* IF true, effect will tick instantly upon application. */
	UPROPERTY(EditAnywhere, Category = "Duration")
		bool bTickOnApplication;
	UPROPERTY(EditAnywhere, Category = "Duration")
		bool bExecuteOnApplication;


	/* 
		Modifier applied to attribute
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAAttributeModifier AtributeModifier;
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FAFAttributeModifierContainer Modifiers;

	UPROPERTY(EditAnywhere, Category = "Execution Type")
		TSubclassOf<class UGAEffectExecution> ExecutionType;

	UPROPERTY(EditAnywhere, Category = "Modifiers")
		TSubclassOf<class UGAEffectExtension> Extension;
	/* 
		Effects applied when this effect is applied. 
		These effects will be applied with the same context and the same target as
		effect, which stores them.
	*/
	UPROPERTY(EditAnywhere, Category = "Linked Effects")
		TArray<TSubclassOf<UGAGameEffectSpec>> OnAppliedEffects;

	/* Effects applied when this effect expire*/
	UPROPERTY(EditAnywhere, Category = "Linked Effects")
		TArray<TSubclassOf<UGAGameEffectSpec>> OnExpiredEffects;

	/* Effects applied when this effect is removed. */
	UPROPERTY(EditAnywhere, Category = "Linked Effects")
		TArray<TSubclassOf<UGAGameEffectSpec>> OnRemovedEffects;

	/* 
		Effects applied only when certain criteria are met.
		Just dumbed here it needs it's own structure that will actually alow to setup those conditions.
	*/
	UPROPERTY(EditAnywhere, Category = "Linked Effects")
		TArray<TSubclassOf<UGAGameEffectSpec>> ConditonalEffects;


	/* Remove effects with these tags. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RemoveEffectWithTags;

	/*
		Tags descrbing what this effect does.
	*/
	/* Tags I own and I don't apply. New tags can be added here as the effect is applied. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OwnedTags;
	/* Tags owned by this effect and not applied. Describe the type of this effect. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer EffectTags;

	/*
		Tags applied to attribute when effect is applying non-instant effect.
		Owned tags of another effect are checked against these tags to calculate
		modifier.
		Also it's checked when tag is applied to determine effect stacking,
		when stacking is set to Override or StrongerOverride.

		Use only single tag here, with proper hierarchy. 
		Using multiple tags should be reserved only for special cases.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer AttributeTags;

	/* If Target have these tags I will not be applied to it. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DenyTags;
	
	/* Applied immunity tags. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer AppliedImmunityTags;

	/* Tags, which are applied to Target when effect is Duration/Periodic based. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ApplyTags;

	/* Tags, required for this effect to be applied. If these tags are not present, effect will be ignored. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RequiredTags;

	/* Tags, required for this effect to be active. If these tags are not present, effect will be ignored. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OngoingRequiredTags;
public:
	UGAGameEffectSpec();
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
		UGAGameEffectSpec* Spec;
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectClass
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Effect")
		TSubclassOf<UGAGameEffectSpec> SpecClass;

	const bool operator==(const FGAEffectClass& Other) const
	{
		return SpecClass == Other.SpecClass;
	}
	const bool operator==(const TSubclassOf<UGAGameEffectSpec>& OtherClass) const
	{
		return SpecClass == OtherClass;
	}
	void operator=(const FGAEffectClass& Other)
	{
		SpecClass = Other.SpecClass;
	}
	void operator=(const TSubclassOf<UGAGameEffectSpec>& Other)
	{
		SpecClass = Other;
	}
	void operator=(UGAGameEffectSpec* Other)
	{
		SpecClass = Other->GetClass();
	}
};

/*
	Special effect container, which contains Effect class and Handle to already instanced effect
	from this class.
*/
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectProperty
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Effect")
		FGAEffectClass SpecClass;

	UPROPERTY()
		class UAFEffectApplicationRequirement* ApplicationRequirement;
	UPROPERTY()
		class UAFEffectCustomApplication* Application;
	UPROPERTY()
		class UGAEffectExecution* Execution;
	UPROPERTY()
		class UGAGameEffectSpec* Spec;

	UPROPERTY()
		float Duration;
	UPROPERTY()
		float Period;
	/* 
		Handle to effect created from SpecClass 
		This handle is only created and kept for instant effects,
		so we don't create new object every time instant effect is created
		as potentially there can be quite a lot of allocations.
	*/
	FGAEffectHandle Handle;
	//target of handle, handle to effect.
	//only used for duration effcts, and cleared after effect is removed.
	TMap<UObject*, FGAEffectHandle> Handles;
	FGAEffectProperty()
		: ApplicationRequirement(nullptr),
		Application(nullptr),
		Execution(nullptr),
		Spec(nullptr)
	{};

	TSubclassOf<UGAGameEffectSpec> GetClass() const { return SpecClass.SpecClass; }
	const TSubclassOf<UGAGameEffectSpec>& GetClassRef() { return SpecClass.SpecClass; }
	UGAGameEffectSpec* GetSpec() const { return SpecClass.SpecClass->GetDefaultObject<UGAGameEffectSpec>(); }

	//intentionally non const.
	FGAEffectHandle& GetHandleRef() { return Handle; }
	void SetHandle(const FGAEffectHandle& InHandle) { Handle = InHandle; };
	void OnEffectRemoved(UObject* InTarget, const FGAEffectHandle& InHandle) {}

	FObjectKey GetClassKey() const
	{
		return FObjectKey(GetClass());
	}

	const bool IsValid() const
	{
		return SpecClass.SpecClass ? true : false;
	}
	const bool IsInitialized() const
	{
		return ApplicationRequirement && Application && Execution && Spec;
	}
	const bool IsValidHandle() const
	{
		return Handle.IsValid();
	}
	const bool operator==(const FGAEffectProperty& Other) const
	{
		return SpecClass == Other.SpecClass;
	}
	const bool operator==(const TSubclassOf<UGAGameEffectSpec>& OtherClass) const
	{
		return SpecClass == OtherClass;
	}
	void operator=(const FGAEffectProperty& Other)
	{
		SpecClass = Other.SpecClass;
		Handle = Other.Handle;
	}
	void operator=(const TSubclassOf<UGAGameEffectSpec>& Other)
	{
		SpecClass = Other;
	}
	void operator=(UGAGameEffectSpec* Other)
	{
		SpecClass = Other->GetClass();
	}
};

/*
	Calculcated magnitudes, captured attributes and tags, set duration.
	Final effect which then is used to apply custom calculations and attribute changes.
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FAFEffectMulicastDelegate, const FGAEffectHandle&);

struct ABILITYFRAMEWORK_API FGAEffect : public TSharedFromThis<FGAEffect>
{
	/* Cached pointer to original effect spec. */
	
	class UGAEffectExecution* Execution;
	UWorld* TargetWorld;
	/* 
		Calculated mods ready to be applied. 
		These are perlimenarly calculcated mods, 
		which can be furhter modified by Calculation object.
	*/

	
	/* Contains all tags gathered on the way to application ? */

	bool IsActive;
public:
	//pointer ? Acces trough handle ?
	class UGAGameEffectSpec* GameEffect;
	FGAEffectContext Context;
	FGameplayTagContainer OwnedTags;
	FGameplayTagContainer ApplyTags;
	FGameplayTagContainer RequiredTags;

	FGAEffectHandle Handle;

	mutable FTimerHandle PeriodTimerHandle;
	mutable FTimerHandle DurationTimerHandle;

	FGAEffectMod AttributeMod;
//because I'm fancy like that and like to make spearate public for fields and functions.
public:
	//Simple delegates to make sure they are bound only to one object.
	FAFEffectMulicastDelegate OnEffectPeriod;
	FAFEffectMulicastDelegate OnEffectExpired;
	FAFEffectMulicastDelegate OnEffectRemoved;

	float AppliedTime;
	float LastTickTime;
public:
	void SetContext(const FGAEffectContext& ContextIn);
	FGAEffectMod GetAttributeModifier();

	class UGAAbilitiesComponent* GetInstigatorComp() { return Context.InstigatorComp.Get(); }
	class UGAAbilitiesComponent* GetTargetComp() { return Context.TargetComp.Get(); }
	inline void AddOwnedTags(const FGameplayTagContainer& TagsIn) { OwnedTags.AppendTags(TagsIn); }
	inline void AddApplyTags(const FGameplayTagContainer& TagsIn) { ApplyTags.AppendTags(TagsIn); }
	void OnApplied();
	void OnDuration();
	void OnExpired();
	void OnRemoved();
	void OnExecuted();
	void DurationExpired();

	float GetDurationTime() const;
	float GetPeriodTime() const;
	float GetCurrentActivationTime();
	float GetCurrentActivationTime() const;
	float GetCurrentTickTime();
	float GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn) const;
	bool IsValid() const
	{
		return GameEffect != nullptr;
	}
	FString ToString()
	{
		if (GameEffect)
		{
			return GameEffect->GetName();
		}
		return FString();
	}
	FGAEffect()
		: GameEffect(nullptr)
	{}
	FGAEffect(class UGAGameEffectSpec* GameEffectIn, 
		const FGAEffectContext& ContextIn);

	~FGAEffect();

private:
	FGAEffectMod GetMod(FGAAttributeModifier& ModInfoIn, class UGAGameEffectSpec* InSpec);
};

/*
	Minimum replicated info about applied info, so we don't replicate full effect if not needed.
	Also provide callbacks for cues assigned to this Effect, so they can be predictevly,
	executed on clients.

	Add replication optimization.
*/

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere)
		float Value;
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
	/*
	If PeriodNum > 0 and Period > 0, then duration of
	effect is PeriodNum * Period
	*/
	UPROPERTY(EditAnywhere, Category = "Duration")
		float PeriodNum;
};
/*
	Effect spec struct, which can be used to create effect specs directly inside abitrary classes ?
*/
USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectSpecDefinition
{
	GENERATED_USTRUCT_BODY()
};

struct FGAActiveGameEffect
{
	FGAActiveGameEffect();
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAInstigatorInstancedEffectContainer
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
			TArray<class UGAEffectExtension*> Effects;
};

/*
	Contains all active effects (which have duration, period or are infinite).

	The only reason this is USTRUCT() is because we need to hold hard references somewhere to instanced
	effects.
*/

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectRepInfo : public FFastArraySerializerItem
{
	GENERATED_USTRUCT_BODY()

public:
	//Handle to effect, which is using this info.
	UPROPERTY()
		FGAEffectHandle Handle;
	UPROPERTY()
		float AppliedTime;
	UPROPERTY()
		float PeriodTime;
	UPROPERTY()
		float Duration;
	UPROPERTY()
		float ReplicationTime;

	FSimpleDelegate OnAppliedDelegate;

	UPROPERTY()
		FGAEffectContext Context;

	void OnApplied();
	void OnPeriod();
	void OnRemoved();

	void PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer);
	void PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer);


	FGAEffectRepInfo()
	{};

	const bool operator==(const FGAEffectRepInfo& Other) const
	{
		return Handle == Other.Handle;
	}

	FGAEffectRepInfo(float AppliedTimeIn, float PeriodTimeIn, float DurationIn, float ReplicationTimeIn)
		: AppliedTime(AppliedTimeIn),
		PeriodTime(PeriodTimeIn),
		Duration(DurationIn),
		ReplicationTime(ReplicationTimeIn)
	{};
};

USTRUCT()
struct ABILITYFRAMEWORK_API FGAGameCue
{
	GENERATED_USTRUCT_BODY()


	//Handle to effect, which spawned this cue.
	UPROPERTY()
		FGAEffectHandle EffectHandle;
};

USTRUCT()
struct ABILITYFRAMEWORK_API FGameCueContainer
{
	GENERATED_USTRUCT_BODY()

	TWeakObjectPtr<UGAAbilitiesComponent> OwningComp;
public:
	void AddCue(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams);
};


USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGAEffectRepInfo> ActiveEffectInfos;

	TMap<FGAAttribute, TSet<FGAEffectHandle>> EffectByAttribute;

	//TMap<FObjectKey, TQueue<FGAEffectHandle>> TestEffectByClass;

	TMap<FObjectKey, TArray<FGAEffectHandle>> EffectByClass;
	//TQueue<FGAEffectHandle> dupa;
	/* All effects. */
	TSet<FGAEffectHandle> ActiveEffectHandles;
	/* 
		Contains effects with infinite duration.
		Infinite effects are considred to be special case, where they can only be self spplied
		and must be manually removed.
	*/
	TSet<FGAEffectHandle> InfiniteEffects;
	
	//not really sure if we really need set.\
	// it could be usefull, for effects which stack in add.
	/*
		UObject* - Instigator
		FName = Effect class name
		FGAEffectHandle = handle to effect of class.
	*/
	TMap<UObject*, TMap<UClass*, TSet<FGAEffectHandle>>> InstigatorEffectByClass;

	/*
		FName = Effect class name
		FGAEffectHandle = handle to effect of class.
	*/
	TMap<UClass*, TSet<FGAEffectHandle>> TargetEffectByClass;

	/* Keeps effects instanced per target actor. */
	UPROPERTY(NotReplicated)
	TArray<class UGAEffectExtension*> TargetInstancedEffects;

	UPROPERTY(NotReplicated)
		class UGAAbilitiesComponent* OwningComponent;
public:
	//FGAEffectContainer();
	FGAEffectHandle ApplyEffect(FGAEffect* EffectIn, FGAEffectProperty& InProperty);
	/* Removesgiven number of effects of the same type. If Num == 0 Removes all effects */
	void RemoveEffect(const FGAEffectProperty& HandleIn, int32 Num = 1);
	/* Removesgiven number of effects of the same type. If Num == 0 Removes all effects */
	void RemoveEffectByHandle(const FGAEffectHandle& InHandle, const FGAEffectProperty& InProperty);
	/* Remove given number of effects of the same type */
	void ApplyReplicationInfo(const FGAEffectHandle& HandleIn);
	inline int32 GetEffectsNum() const { return ActiveEffectHandles.Num(); };

	EGAEffectAggregation GetEffectAggregation(const FGAEffectHandle& HandleIn) const;

	TSet<FGAEffectHandle> GetHandlesByAttribute(const FGAEffectHandle& HandleIn);

	TSet<FGAEffectHandle> GetHandlesByClass(const FGAEffectProperty& InProperty,
		const FGAEffectContext& InContext);

	void AddEffect(const FGAEffectHandle& HandleIn, bool bInfinite = false);
	void AddEffectByClass(const FGAEffectHandle& HandleIn);

	void RemoveFromAttribute(const FGAEffectHandle& HandleIn);
	void RemoveEffectProtected(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	void RemoveInstigatorEffect(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	void RemoveTargetEffect(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	void ApplyEffectInstance(class UGAEffectExtension* EffectIn);
	//modifiers
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
	bool IsEffectActive(const FGAEffectHandle& HandleIn);
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		//return FastArrayDeltaSerialize<FGAEffectRepInfo>(ActiveEffectInfos, DeltaParms, *this);
		return FFastArraySerializer::FastArrayDeltaSerialize<FGAEffectRepInfo, FGAEffectContainer>(ActiveEffectInfos, DeltaParms, *this);
	}
	UWorld* GetWorld() const;
	
};
template<>
struct TStructOpsTypeTraits< FGAEffectContainer > : public TStructOpsTypeTraitsBase2<FGAEffectContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
		WithCopy = false
	};
};
