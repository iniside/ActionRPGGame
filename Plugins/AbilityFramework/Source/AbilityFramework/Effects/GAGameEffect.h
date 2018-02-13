#pragma once
//#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.h"
#include "GameplayTagContainer.h"
//#include "NetSerialization.h"
#include "GAGameEffect.generated.h"

DECLARE_STATS_GROUP(TEXT("GameEffect"), STATGROUP_GameEffect, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("GatherModifiers"), STAT_GatherModifiers, STATGROUP_GameEffect, );

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

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FAFCueContainer
{
	GENERATED_BODY()
public:
	/* Cues to apply */
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer CueTags;
};

USTRUCT(BlueprintType)
struct FAFConditionalEffectContainer
{
	GENERATED_BODY()
public:
	/* If target have this tag apply specified effects */
	UPROPERTY(EditAnywhere)
		FGameplayTag RequiredTag;

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<class UGAGameEffectSpec>> Effects;
};


/*
	Base effect class. You can derive your own specialized classes from it
	with preset customizations and values. You should never directly inherit blueprints from it.
*/
UCLASS(Blueprintable, BlueprintType, Abstract)
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

	UPROPERTY(EditAnywhere, meta=(UseDisplayName), Category = "Effect Info")
		TSubclassOf<class UAFEffectApplicationRequirement> ApplicationRequirement;
	/* 
		How effect should stack. Only relevelant for periodic effects
		and modifiers applied on period.
	*/
	UPROPERTY(EditAnywhere, meta = (UseDisplayName), Category = "Effect Info")
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
	/* Duration of single period. */
	UPROPERTY(EditAnywhere, Category = "Period")
		FGAMagnitude Period;
	/* Total duration of effect (if applicable) */
	UPROPERTY(EditAnywhere, Category = "Duration")
		float MaxStackedDuration;

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

	/* Cues to apply by this effect. */
	UPROPERTY(EditAnywhere, Category = "Cues")
		FAFCueContainer Cues;
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

	UPROPERTY(EditAnywhere, Category = "Event Tags")
		FGameplayTag OnExpiredEvent;
	UPROPERTY(EditAnywhere, Category = "Event Tags")
		FGameplayTag OnPeriodEvent;
	UPROPERTY(EditAnywhere, Category = "Event Tags")
		FGameplayTag OnRemovedEvent;


	UPROPERTY(EditAnywhere, Category = "Event Application Tags")
		FGameplayTag OnEffectApplyToTargetEvent;
	UPROPERTY(EditAnywhere, Category = "Event Application Tags")
		FGameplayTag OnEffectApplyToSelfEvent;

	/* 
		Effects applied only when certain criteria are met.
		Just dumbed here it needs it's own structure that will actually alow to setup those conditions.
	*/
	UPROPERTY(EditAnywhere, Category = "Linked Effects")
		FAFConditionalEffectContainer IfHaveTagEffect;


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
		When effect is appied trigger events with these tags.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer AppliedEventTags;
	/*
		When effect is executed trigger events with these tags.
	*/
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ExecuteEventTags;

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

	/* Tags, required for this effect to be executed. If these tags are not present, effect will be ignored. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ExecutionRequiredTags;
public:
	UGAGameEffectSpec();
};
/*
	Base effect class to extend from when creating effect blueprints.
*/
UCLASS(Blueprintable, BlueprintType, Abstract)
class ABILITYFRAMEWORK_API UAFEffectSpec : public UGAGameEffectSpec
{
	GENERATED_BODY()
public:
};

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectClass
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Effect")
		TSubclassOf<UGAGameEffectSpec> SpecClass;

	FGAEffectClass()
	{}
	FGAEffectClass(TSubclassOf<UGAGameEffectSpec> InClass)
		: SpecClass(InClass)
	{}
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
protected:
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
		FAFPredictionHandle PredictionHandle;
protected:
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
	/*
		Holds handle to duration/infinite effects. Since there can be multiple effects active on multiple targets.

	*/
protected:
	//possibly multiple handles per target ?
	TMap<FObjectKey, FGAEffectHandle> Handles;
	TMap<FGAEffectHandle, UObject*> HandleToTarget;
	TMap<FGAEffectHandle, FGAEffectContext*> Contexts;
	TMap<FGAEffectHandle, FAFCueHandle> EffectCues;
public:
	FGAEffectProperty()
		: ApplicationRequirement(nullptr),
		Application(nullptr),
		Execution(nullptr),
		Spec(nullptr),
		Duration(0),
		Period(0)
	{};

	FGAEffectProperty(TSubclassOf<UGAGameEffectSpec> InClass)
		: SpecClass(InClass),
		ApplicationRequirement(nullptr),
		Application(nullptr),
		Execution(nullptr),
		Spec(nullptr),
		Duration(0),
		Period(0)
	{};

	TSubclassOf<UGAGameEffectSpec> GetClass() const { return SpecClass.SpecClass; }
	const TSubclassOf<UGAGameEffectSpec>& GetClassRef() { return SpecClass.SpecClass; }
	UGAGameEffectSpec* GetSpec() const { return Spec; }

	bool CanApply(FGAEffect* EffectIn
		, struct FGAEffectContainer* InContainer
		, const FGAEffectContext& InContext
		, const FGAEffectHandle& InHandle);

	bool ApplyEffect(const FGAEffectHandle& InHandle
		, struct FGAEffect* EffectIn
		, struct FGAEffectContainer* InContainer
		, const FGAEffectContext& InContext
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	void ExecuteEffect(const FGAEffectHandle& InHandle
		, const FGAEffectContext& InContext
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	void EffectExecution(const FGAEffectHandle& HandleIn
		, FGAEffectMod& ModIn
		, FGAEffectContext& Context
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	//intentionally non const.
	//FGAEffectHandle& GetHandleRef() { return Handle; }
	//void SetHandle(const FGAEffectHandle& InHandle) { Handle = InHandle; };
	void OnEffectRemoved(UObject* InTarget, const FGAEffectHandle& InHandle) {}

	void Initialize();
	void InitializeIfNotInitialized(const FGAEffectContext& InContext);

	inline float GetPeriod() const
	{
		return Period;
	}
	inline float GetDuration() const
	{
		return Duration;
	}
	void AddHandle(UObject* Target, const FGAEffectHandle& InHandle)
	{
		FObjectKey key(Target);
		Handles.Add(key, InHandle);
		HandleToTarget.Add(InHandle, Target);
	}

	FGAEffectHandle GetHandle(UObject* From)
	{
		FObjectKey key(From);
		if (FGAEffectHandle* handle = Handles.Find(key))
			return *handle;

		return FGAEffectHandle();
	}
	FGAEffectHandle GetHandle(UObject* From) const
	{
		FObjectKey key(From);
		if (const FGAEffectHandle* handle = Handles.Find(key))
			return *handle;

		return FGAEffectHandle();
	}

	inline FAFPredictionHandle GetPredictionHandle() const
	{
		return PredictionHandle;
	}

	bool IsHandleValid(UObject* For)
	{
		FObjectKey Key(For);
		FGAEffectHandle* Handle = Handles.Find(Key);
		if (Handle)
		{
			return Handle->IsValid();
		}
		return false;
	}
	bool IsHandleValid(UObject* For) const
	{
		FObjectKey Key(For);
		const FGAEffectHandle* Handle = Handles.Find(Key);
		if (Handle)
		{
			return Handle->IsValid();
		}
		return false;
	}
	void RemoveHandle(const FGAEffectHandle& InHandle)
	{
		UObject* Target = nullptr;
		HandleToTarget.RemoveAndCopyValue(InHandle, Target);
		if(Target)
		{
			FObjectKey key(Target);
			Handles.Remove(key);
			Contexts.Remove(InHandle);
		}
	}

	void SetPredictionHandle(const FAFPredictionHandle& InHandle)
	{
		PredictionHandle = InHandle;
	}

	FGAAttributeModifier& GetAttributeModifier()
	{
		return Spec->AtributeModifier;
	}

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
	/*const bool IsValidHandle() const
	{
		return Handle.IsValid();
	}*/
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
		//Handle = Other.Handle;
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


struct FAFStatics
{
	static float GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn
		, const FGAEffectContext& InContext
		, const FGAEffectHandle& InHandle);
	static FGAEffectMod GetAttributeModifier(FGAAttributeModifier& ModInfoIn
		, class UGAGameEffectSpec* InSpec,
		const FGAEffectContext& InContext,
		const FGAEffectHandle& InHandle);
};

/*
	Calculcated magnitudes, captured attributes and tags, set duration.
	Final effect which then is used to apply custom calculations and attribute changes.
*/
DECLARE_MULTICAST_DELEGATE_OneParam(FAFEffectMulicastDelegate, const FGAEffectHandle&);
UENUM()
enum class ERepInfoType : uint8
{
	LocallyPredicted,
	RemotePredicted,
	Server
};
USTRUCT()
struct ABILITYFRAMEWORK_API FGAEffect : public FFastArraySerializerItem//, public TSharedFromThis<FGAEffect>
{

	GENERATED_BODY()
	/* Cached pointer to original effect spec. */
	
	UPROPERTY(NotReplicated)
		class UGAEffectExtension* Extension;

	UWorld* TargetWorld;
	/* 
		Calculated mods ready to be applied. 
		These are perlimenarly calculcated mods, 
		which can be furhter modified by Calculation object.
	*/
public:
	
	//Spec from which this effect originated.
	UPROPERTY()
		TSubclassOf<UGAGameEffectSpec> GameEffectClass;
	UPROPERTY()
		FAFPredictionHandle PredictionHandle;
	UPROPERTY()
		FGAEffectContext Context;
	
	FGameplayTagContainer OwnedTags;
	FGameplayTagContainer ApplyTags;
	FGameplayTagContainer RequiredTags;

	FGAEffectHandle Handle;

	mutable FTimerHandle PeriodTimerHandle;
	mutable FTimerHandle DurationTimerHandle;

	//FGAEffectMod AttributeMod;
//because I'm fancy like that and like to make spearate public for fields and functions.
public:
	float AppliedTime;
	float LastTickTime;
public:
	void PreReplicatedRemove(const struct FGAEffectContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FGAEffectContainer& InArraySerializer);
	void PostReplicatedChange(const struct FGAEffectContainer& InArraySerializer);

	void SetContext(const FGAEffectContext& ContextIn);

	class UAFAbilityComponent* GetInstigatorComp() { return Context.InstigatorComp.Get(); }
	class UAFAbilityComponent* GetTargetComp() { return Context.TargetComp.Get(); }
	inline void AddOwnedTags(const FGameplayTagContainer& TagsIn) { OwnedTags.AppendTags(TagsIn); }
	inline void AddApplyTags(const FGameplayTagContainer& TagsIn) { ApplyTags.AppendTags(TagsIn); }
	void OnApplied();
	void OnExpired();
	void OnRemoved();
	void OnExecuted();

	float GetDurationTime() const;
	float GetPeriodTime() const;
	float GetCurrentDuration() const;
	float GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn) const;

	UGAGameEffectSpec* GetEffect()
	{
		return GameEffectClass.GetDefaultObject();
	}
	UGAGameEffectSpec* GetEffect() const
	{
		return GameEffectClass.GetDefaultObject();
	}
	bool IsValid() const
	{
		return GameEffectClass != nullptr;
	}
	FString ToString()
	{
		if (GameEffectClass)
		{
			return GameEffectClass->GetName();
		}
		return FString();
	}
	FGAEffect()
		: GameEffectClass(nullptr)
	{}
	FGAEffect(TSubclassOf<class UGAGameEffectSpec> GameEffectIn, 
		const FGAEffectContext& ContextIn);

	~FGAEffect();


	bool operator==(const FGAEffect& Other) const
	{
		return Handle == Other.Handle;
	}
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

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAInstigatorInstancedEffectContainer
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
			TArray<class UGAEffectExtension*> Effects;
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

	TWeakObjectPtr<UAFAbilityComponent> OwningComp;
public:
	void AddCue(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams);
};


USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGAEffectContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGAEffect> ActiveEffectInfos;
	
	UPROPERTY()
	TMap<FAFPredictionHandle, FGAEffectHandle> HandleByPrediction;
	
	UPROPERTY()
	TMap<FGAEffectHandle, FAFPredictionHandle> PredictionByHandle;

	
	TMap<FAFPredictionHandle, FGAEffect*> PredictedEffectInfos;

	
	TMap<FGAAttribute, TSet<FGAEffectHandle>> EffectByAttribute;

	
	TMap<FObjectKey, TArray<FGAEffectHandle>> EffectByClass;

	//TQueue<FGAEffectHandle> dupa;
	/* All effects. */
	UPROPERTY()
	TSet<FGAEffectHandle> ActiveEffectHandles;
	/* 
		Contains effects with infinite duration.
		Infinite effects are considred to be special case, where they can only be self spplied
		and must be manually removed.
	*/
	UPROPERTY()
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

	UPROPERTY(NotReplicated)
		class UAFEffectsComponent* OwningComponent;
public:
	
	/*
	* @call Order:
	* Previous Function: UAFAbilityComponent::ApplyEffectToSelf 
	* Next Function: InProperty.Application->ApplyEffect();
	* FGAEffectContainer::ApplyReplicationInfo()
	* 
	* Apply target to Me. Try to apply effect to container and launch Events in:
	* TMap<FGameplayTag, FSimpleDelegate> OnEffectEvent - event is called before application;
	* TMap<FGameplayTag, FSimpleDelegate> OnEffectApplyToSelf - event is called before application;
	*
	* @param EffectIn* - Effect to apply
	* @param InProperty - cached effect information
	* @param InContext - Context about effect application. Target, instigator, causer.
	* @param Modifier - optional modifier which can be applied to effect.
	* @return Handle to Effect @ UAFAbilityComponent::ApplyEffectToSelf 
	*/
	FGAEffectHandle ApplyEffect(FGAEffect* EffectIn, FGAEffectProperty& InProperty
		, const FGAEffectContext& InContext
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	/* Removesgiven number of effects of the same type. If Num == 0 Removes all effects */
	TArray<FGAEffectHandle> RemoveEffect(const FGAEffectProperty& HandleIn, int32 Num = 1);
	/* Removesgiven number of effects of the same type. If Num == 0 Removes all effects */
	void RemoveEffectByHandle(const FGAEffectHandle& InHandle, const FGAEffectProperty& InProperty);

	inline int32 GetEffectsNum() const { return ActiveEffectHandles.Num(); };

	EGAEffectAggregation GetEffectAggregation(const FGAEffectHandle& HandleIn) const;

	TSet<FGAEffectHandle> GetHandlesByAttribute(const FGAEffectHandle& HandleIn);

	TSet<FGAEffectHandle> GetHandlesByClass(const FGAEffectProperty& InProperty,
		const FGAEffectContext& InContext);

	void AddEffect(FGAEffectProperty& InProperty, const FGAEffectHandle& HandleIn, bool bInfinite = false);
	void AddEffectByClass(const FGAEffectHandle& HandleIn);

	void RemoveFromAttribute(const FGAEffectHandle& HandleIn);
	void RemoveEffectProtected(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	void RemoveInstigatorEffect(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	void RemoveTargetEffect(const FGAEffectHandle& HandleIn, const FGAEffectProperty& InProperty);
	//modifiers
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
	bool IsEffectActive(const FGAEffectHandle& HandleIn);
	bool IsEffectActive(const FGAEffectHandle& HandleIn) const;
	bool ContainsEffectOfClass(const FGAEffectProperty& InProperty);

	void ApplyFromPrediction(const FGAEffectHandle& InHandle, const FAFPredictionHandle& InPredHandle);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		//return FastArrayDeltaSerialize<FGAEffectRepInfo>(ActiveEffectInfos, DeltaParms, *this);
		return FFastArraySerializer::FastArrayDeltaSerialize<FGAEffect, FGAEffectContainer>(ActiveEffectInfos, DeltaParms, *this);
	}

	const TSet<FGAEffectHandle>& GetAllEffectHandles() const
	{
		return ActiveEffectHandles;
	}

	UWorld* GetWorld() const;

	///Helpers
	float GetRemainingTime(const FGAEffectHandle& InHandle) const
	{
		if (InHandle.GetEffectPtr().IsValid())
		{
			float Duration = InHandle.GetEffectPtr()->GetDurationTime();
			return FMath::Clamp<float>(Duration - InHandle.GetEffectPtr()->GetCurrentDuration(), 0, Duration);
		}
		return 0;
	}
	float GetRemainingTimeNormalized(const FGAEffectHandle& InHandle) const
	{
		if (InHandle.GetEffectPtr().IsValid())
		{
			float CurrentDuration = InHandle.GetEffectPtr()->GetCurrentDuration();
			float MaxDuration = InHandle.GetEffectPtr()->GetDurationTime();
			return FMath::Clamp<float>(CurrentDuration * 1 / MaxDuration, 0, 1);
		}
		return 0;
	}
	/* Get Current effect ime clamped to max duration */
	float GetCurrentTime(const FGAEffectHandle& InHandle) const
	{
		if(InHandle.GetEffectPtr().IsValid())
			return InHandle.GetEffectPtr()->GetCurrentDuration();
		return 0;
	}
	float GetCurrentTimeNormalized(const FGAEffectHandle& InHandle) const
	{
		if (InHandle.GetEffectPtr().IsValid())
		{
			float CurrentDuration = InHandle.GetEffectPtr()->GetCurrentDuration();
			float MaxDuration = InHandle.GetEffectPtr()->GetDurationTime();
			return CurrentDuration * 1 / MaxDuration;
		}
		return 0;
	}
	float GetEndTime(const FGAEffectHandle& InHandle) const
	{
		return 0;
	}
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
