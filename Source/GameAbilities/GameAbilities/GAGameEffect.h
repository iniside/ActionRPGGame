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
struct GAMEABILITIES_API FGAMagnitude
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
	
	UPROPERTY(EditAnywhere)
		FGASummedAttributeBasedModifier SummedAttributeBased;
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
struct GAMEABILITIES_API FGAAttributeModifier
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

UCLASS(Blueprintable, BlueprintType, EditInLineNew)
class GAMEABILITIES_API UGAGameEffectSpec : public UObject
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
		EGAEffectType EffectType;
	/* 
		How effect should stack. Only relevelant for periodic effects
		and modifiers applied on period.
	*/
	UPROPERTY(EditAnywhere, Category = "Effect Info")
		EGAEffectStacking EffectStacking;

	/*
		Who should aggregate this effect. Relevelant for stacking type
		and by this only relevelant for periodic effects.
	*/
	UPROPERTY(EditAnywhere, Category = "Effect Info")
		EGAEffectAggregation EffectAggregation;

	/* Total duration of effect (if applicable) */
	UPROPERTY(EditAnywhere, Category = "Duration")
		FGAMagnitude Duration;
	/* Duration of single period. */
	UPROPERTY(EditAnywhere, Category = "Duration")
		FGAMagnitude Period;
	/* 
		Modifier applied to attribute
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		FGAAttributeModifier AtributeModifier;

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

	/*
		Tags descrbing what this effect does.
	*/
	/* Tags I own and I don't apply */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OwnedTags;

	/* If Target have these tags I will not be applied to it. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DenyTags;

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
struct GAMEABILITIES_API FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
		UGAGameEffectSpec* Spec;
};

struct FGAEffectBase
{

};

struct FGAEffectActive
{
	
};

/*
	Calculcated magnitudes, captured attributes and tags, set duration.
	Final effect which then is used to apply custom calculations and attribute changes.
*/
struct GAMEABILITIES_API FGAEffect : public TSharedFromThis<FGAEffect>
{
	/* Cached pointer to original effect spec. */
	class UGAGameEffectSpec* GameEffect;
	class UGAEffectExecution* Execution;
	UWorld* TargetWorld;
	/* 
		Calculated mods ready to be applied. 
		These are perlimenarly calculcated mods, 
		which can be furhter modified by Calculation object.
	*/

	FGAEffectContext Context;
	/* Contains all tags gathered on the way to application ? */

	bool IsActive;
public:
	FGameplayTagContainer OwnedTags;
	FGameplayTagContainer ApplyTags;
	FGameplayTagContainer RequiredTags;

	FGAEffectHandle Handle;

	FTimerHandle PeriodTimerHandle;
	FTimerHandle DurationTimerHandle;
	/* Spawmed by which ability. */
	TWeakObjectPtr<class UGAAbilityBase> Ability;
//because I'm fancy like that and like to make spearate public for fields and functions.
public:
	//Simple delegates to make sure they are bound only to one object.
	FSimpleDelegate OnEffectPeriod;
	FSimpleDelegate OnEffectExpired;
	FSimpleDelegate OnEffectRemoved;

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

	void DurationExpired();

	float GetDurationTime();
	float GetPeriodTime();
	float GetCurrentActivationTime();
	float GetCurrentActivationTime() const;
	float GetCurrentTickTime();
	float GetFloatFromAttributeMagnitude(const FGAMagnitude& AttributeIn);
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
	FGAEffectMod GetMod(FGAAttributeModifier& ModInfoIn);
};

/*
	Minimum replicated info about applied info, so we don't replicate full effect if not needed.
	Also provide callbacks for cues assigned to this Effect, so they can be predictevly,
	executed on clients.

	Add replication optimization.
*/

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGAEffectAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere)
		float Value;
	UPROPERTY(EditAnywhere)
		EGAAttributeChangeType ChangeType;
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
struct GAMEABILITIES_API FGAEffectSpecDefinition
{
	GENERATED_USTRUCT_BODY()
};

struct FGAActiveGameEffect
{
	FGAActiveGameEffect();
};

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGAInstigatorInstancedEffectContainer
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
struct GAMEABILITIES_API FGAEffectRepInfo : public FFastArraySerializerItem
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
struct GAMEABILITIES_API FGAGameCue
{
	GENERATED_USTRUCT_BODY()


	//Handle to effect, which spawned this cue.
	UPROPERTY()
		FGAEffectHandle EffectHandle;
};

USTRUCT()
struct GAMEABILITIES_API FGameCueContainer
{
	GENERATED_USTRUCT_BODY()

	TWeakObjectPtr<UGAAbilitiesComponent> OwningComp;
public:
	void AddCue(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams);
};
USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGAEffectContainer : public FFastArraySerializer
{
	GENERATED_USTRUCT_BODY()
public:
	TWeakObjectPtr<class UGAAbilitiesComponent> OwningComp;

	UPROPERTY()
		TArray<FGAEffectRepInfo> ActiveEffectInfos;

	TMap<FGAAttribute, TSet<FGAEffectHandle>> EffectByAttribute;

	TMap<FGAEffectHandle, TSharedPtr<FGAEffect>> ActiveEffects;

	/* 
		Contains effects with infinite duration.
		Infinite effects are considred to be special case, where they can only be self spplied
		and must be manually removed.
	*/
	TMap<FGAEffectHandle, TSharedPtr<FGAEffect>> InfiniteEffects;
	/*
		Both of these maps are for determining how effects should stack.
	*/
	/* Effects grouped by their instigator. */
	TMap<UObject*, TMap<FGAEffectHandle, TSharedPtr<FGAEffect>>> InstigatorEffects;
	/* Groups effects by their class, needed to handle overriding and effect stacking. */
	//TMap<UObject*, TMap<UClass*, FGAEffectHandle>> InstigatorEffectHandles;
	
	//not really sure if we really need set.\
	// it could be usefull, for effects which stack in add.
	/*
		UObject* - Instigator
		FName = Effect class name
		FGAEffectHandle = handle to effect of class.
	*/
	TMap<UObject*, TMap<FName, TSet<FGAEffectHandle>>> InstigatorEffectHandles;

	/* Effects grouped by their targer */
	TMap<FGAEffectHandle, TSharedPtr<FGAEffect>> TargetEffects;
	/*
		FName = Effect class name
		FGAEffectHandle = handle to effect of class.
	*/
	TMap<FName, FGAEffectHandle> TargetEffectByType;

	/* Add Handle for instanced effects ? */
	/* Keeps effects instanced per instigator */
	UPROPERTY(NotReplicated)
	TMap<UObject*, FGAInstigatorInstancedEffectContainer> InstigatorInstancedEffects;

	/* Keeps effects instanced per target actor. */
	UPROPERTY(NotReplicated)
	TArray<class UGAEffectExtension*> TargetInstancedEffects;

	UPROPERTY(NotReplicated)
		class UGAAbilitiesComponent* OwningComponent;
public:
	FGAEffectContainer();

	void ApplyEffect(const FGAEffect& EffectIn, const FGAEffectHandle& HandleIn);
	void RemoveEffect(FGAEffectHandle& HandleIn);
	void ApplyReplicationInfo(const FGAEffectHandle& HandleIn);

	void ApplyEffectInstance(class UGAEffectExtension* EffectIn);
	void RemoveEffectInstance(class UGAEffectExtension* EffectIn);
	//modifiers
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
	bool IsEffectActive(const FGAEffectHandle& HandleIn);
protected:
	void InternalApplyPeriodic(const FGAEffectHandle& HandleIn);
	void InternalApplyDuration(const FGAEffectHandle& HandleIn);
	void InternalApplyEffectTags(const FGAEffectHandle& HandleIn);
	void InternalCheckDurationEffectStacking(const FGAEffectHandle& HandleIn);
	void InternalCheckPeriodicEffectStacking(const FGAEffectHandle& HandleIn);
	void InternalApplyEffectByAggregation(const FGAEffectHandle& HandleIn);
	void InternalApplyEffect(const FGAEffectHandle& HandleIn);
	void InternalExtendEffectDuration(const FGAEffectHandle& HandleIn, const FGAEffectHandle& ExtendingHandleIn);
	void InternalCheckInfiniteEffectStacking(const FGAEffectHandle& HandleIn);
	void InternalApplyInfiniteEffect(const FGAEffectHandle& HandleIn);
	bool RemoveWeakerEffect(const FGAEffectHandle& HandleIn);
	bool RemoveEffectsByTags(const FGameplayTagContainer& TagsIn);
	int32 RemoveOverrideEffects(const FGAEffectHandle& HandleIn);
	int32 RemoveStrongerOverrideEffects(const FGAEffectHandle& HandleIn);
	TSharedPtr<FGAEffect> RemoveEffectByAggregation(const FGAEffectHandle& HandleIn);
	FGAEffectHandle* FindHandlePtrByAggregation(const FGAEffectHandle& HandleIn);
	TSharedPtr<FGAEffect> GetEffectByHandle(const FGAEffectHandle& HandleIn);
public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		//return FastArrayDeltaSerialize<FGAEffectRepInfo>(ActiveEffectInfos, DeltaParms, *this);
		return FFastArraySerializer::FastArrayDeltaSerialize<FGAEffectRepInfo, FGAEffectContainer>(ActiveEffectInfos, DeltaParms, *this);
	}
	UWorld* GetWorld() const;
	
};
template<>
struct TStructOpsTypeTraits< FGAEffectContainer > : public TStructOpsTypeTraitsBase
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};
