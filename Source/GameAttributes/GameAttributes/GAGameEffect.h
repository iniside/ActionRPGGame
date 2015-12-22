#pragma once
#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.h"
#include "GAGameEffect.generated.h"

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

/*
	Modifier which can modify other incoming effect. Modify their attributes, trigger other effects etc.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAGameEffectModifier
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		If there is specified attribute, modifier will only be applied to this attribute.
		All tag based effect modifiers, will be ignored.
	*/
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere)
		float Value;
	UPROPERTY(EditAnywhere)
		EGAAttributeMod ModType;

	UPROPERTY(EditAnywhere)
		EGAModifierDirection Direction;
	/*
		Tags, which incoming/outgoing effect must have to be affected by this modifier 
		(in case of tag based modifier).
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAAttributeModifier
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		EGAAttributeMod AttributeMod;

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

	/*
	Who will be target of this attribute change.
	Instigator - pawn which applied effect (regardless of to whom).
	Target - targeted pawn (regardless of who applied).
	*/
	UPROPERTY(EditAnywhere)
		EGAModifierTarget ModifierTarget;

	/*
	These tags must be present on target for this modifier to be applied.
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;
	/*
	None of these tags must be present on target to apply this modifier.
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer DenyTags;
	/*
		How modifier will be executed on target.
	*/
	UPROPERTY(EditAnywhere, Category = "Execution Type")
		TSubclassOf<class UGAEffectExecution> ExecutionType;
};

UCLASS(Blueprintable, BlueprintType, EditInLineNew)
class GAMEATTRIBUTES_API UGAGameEffectSpec : public UObject
{
	GENERATED_BODY()
public:
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
		float Duration;
	/* Duration of single period. */
	UPROPERTY(EditAnywhere, Category = "Duration")
		float Period;
	/* 
		Modifiers applied upon application of effect. They are treated as instant effect
		as never will be applied for duration.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAAttributeModifier> OnAppliedInfo;

	UPROPERTY(EditAnywhere, Category = "Execution Type")
		TSubclassOf<class UGAEffectExecution> ExecutionType;

	UPROPERTY(EditAnywhere, Category = "Modifiers")
		TArray<FGAGameEffectModifier> Modifiers;

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
		Effects also need special behaviors, like search in radius, 
		Complex checking for attributes values, and passing data from and to other effects
		(I deal 40hp damage, other effect might want to apply energy damage equal to 40hp*2
	*/

	/* Tags I own and I don't apply */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OwnedTags;

	/* If Target have these tags I will not be applied to it. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DenyTags;
public:
	UGAGameEffectSpec();
};
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Instanced, Category = "Effect")
		UGAGameEffectSpec* Spec;
};
struct FGAGameEffectMod
{
	FGAAttribute Attribute;
	float Value;
	EGAAttributeChangeType ChangeType;
	FGAGameEffectMod()
		: Attribute(NAME_None),
		Value(0),
		ChangeType(EGAAttributeChangeType::Invalid)
	{};

	FGAGameEffectMod(const FGAAttribute& AttributeIn, float ValueIn, EGAAttributeChangeType ChangeTypeIn)
		: Attribute(AttributeIn),
		Value(ValueIn),
		ChangeType(ChangeTypeIn)
	{};
	
};

struct FGAGameEffectBase
{

};

struct FGAGameEffectActive
{
	
};

/*
	Calculcated magnitudes, captured attributes and tags, set duration.
	Final effect which then is used to apply custom calculations and attribute changes.
*/
struct GAMEATTRIBUTES_API FGAGameEffect : public TSharedFromThis<FGAGameEffect>
{
	/* Cached pointer to original effect spec. */
	class UGAGameEffectSpec* GameEffect;
	class UGAEffectExecution* Execution;
	/* 
		Calculated mods ready to be applied. 
		These are perlimenarly calculcated mods, 
		which can be furhter modified by Calculation object.


	*/

	FGAEffectContext Context;
	FGameplayTagContainer OwnedTags;

	struct FGAGameEffectHandle* Handle;

	FTimerHandle PeriodTimerHandle;
	FTimerHandle DurationTimerHandle;
public:
	void SetContext(const FGAEffectContext& ContextIn);
	TArray<FGAEffectMod> GetOnAppliedMods();
	TArray<FGAEffectMod> GetOnPeriodMods();
	TArray<FGAEffectMod> GetOnDurationMods();

	class UGAAttributeComponent* GetInstigatorComp() { return Context.InstigatorComp.Get(); }
	class UGAAttributeComponent* GetTargetComp() { return Context.TargetComp.Get(); }

	void OnDuration();
	void OnExpired() {}; //internal timer
	void OnRemoved() {}; //internal timer

	void DurationExpired();

	bool IsValid() const
	{
		return GameEffect != nullptr;
	}
	FGAGameEffect()
		: GameEffect(nullptr)
	{}
	FGAGameEffect(class UGAGameEffectSpec* GameEffectIn, 
		const FGAEffectContext& ContextIn);
private:
	TArray<FGAEffectMod> GetMods(TArray<FGAAttributeModifier>& ModsInfoIn);
};

USTRUCT(BlueprintType)
struct FGAGameEffectHandle
{
	GENERATED_USTRUCT_BODY()
protected:
	UPROPERTY() //we will replicate handle, so we need it be uproperty.
		int32 Handle; //Fname Guid ?

	TSharedPtr<FGAGameEffect> EffectPtr;
public:
	bool IsValid() const
	{
		return Handle != INDEX_NONE && EffectPtr.IsValid();
	}

	inline FGAEffectContext& GetContextRef() { return EffectPtr->Context; }
	inline FGAEffectContext& GetContextRef() const { return EffectPtr->Context; }
	
	inline UGAGameEffectSpec* GetEffectSpec() { return EffectPtr->GameEffect; }
	inline UGAGameEffectSpec* GetEffectSpec() const { return EffectPtr->GameEffect; }
	
	inline int32 GetHandle() const { return Handle; }

	inline FGAGameEffect GetEffect() { return *EffectPtr.Get(); }

	inline FGAGameEffect& GetEffectRef() { return EffectPtr.ToSharedRef().Get(); }
	//no kurwa!
	inline FGAGameEffect& GetEffectRef() const { return EffectPtr.ToSharedRef().Get(); };

	inline TSharedPtr<FGAGameEffect> GetEffectPtr() { return EffectPtr; };
	inline TSharedPtr<FGAGameEffect> GetEffectPtr() const { return EffectPtr; };

	inline void SetContext(const FGAEffectContext& ContextIn) { EffectPtr->SetContext(ContextIn); }

	inline FGAEffectContext& GetContext() { return EffectPtr->Context; }
	inline FGAEffectContext& GetContext() const { return EffectPtr->Context; }
	/* Executes effect trough provided execution class. */
	void ExecuteEffect(FGAGameEffectHandle& HandleIn, FGAEffectMod& ModIn, FGAExecutionContext& Context);

	static FGAGameEffectHandle GenerateHandle(FGAGameEffect* EffectIn);

	bool operator==(const FGAGameEffectHandle& Other) const
	{
		return Handle == Other.Handle;
	}
	bool operator!=(const FGAGameEffectHandle& Other) const
	{
		return Handle != Other.Handle;
	}
	friend uint32 GetTypeHash(const FGAGameEffectHandle& InHandle)
	{
		return InHandle.Handle;
	}

	FGAGameEffectHandle()
		: Handle(INDEX_NONE)
	{}

	FGAGameEffectHandle(int32 HandleIn, FGAGameEffect* EffectIn)
		: Handle(HandleIn),
		EffectPtr(EffectIn)
	{
	}
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectAttributeModifier
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
struct FGAGameEffectDuration
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
struct GAMEATTRIBUTES_API FGAGameEffectSpecDefinition
{
	GENERATED_USTRUCT_BODY()
};

/*
	Contains modifiers for effects, which can modify other effects properties.
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAGameEffectModifierContainer
{
	GENERATED_USTRUCT_BODY()
public:
	void GetQualifableMods(const FGAGameEffect& EffectIn) {};
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
};


struct GAMEATTRIBUTES_API FGAGameModifierStack
{
	float Additive;
	float Subtractive;
	float Multiply;
	float Divide;

	FGAGameModifierStack()
		: Additive(0),
		Subtractive(0),
		Multiply(0),
		Divide(0)
	{};
};

struct GAMEATTRIBUTES_API FGACalculationContext
{
	class UGAAttributeComponent* AttributeComp;

	class UGAAttributesBase* Attributes;
	
	EGAModifierDirection Direction;

	FGAGameModifierStack GetModifiers(const FGAGameEffect& EffectIn
		, const FGACalculationContext& Context);

	FGACalculationContext()
	{};
	FGACalculationContext(class UGAAttributeComponent* AttrComp, 
		class UGAAttributesBase* AttrIn,
		EGAModifierDirection DirectionIn)
		: AttributeComp(AttrComp),
		Attributes(AttrIn),
		Direction(DirectionIn)
	{};

};


struct FGAActiveGameEffect
{
	FGAActiveGameEffect();
};
/*
	Contains all active effects (which have duration, period or are infinite).
*/
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAGameEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	TWeakObjectPtr<class UGAAttributeComponent> OwningComp;
	TArray<TArray<FGAGameEffectModifier>> Modifiers;

	TArray<FGAGameEffectHandle> EffectsHandles;

	TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>> ActiveEffects;
	/*
		Both of these maps are for determining how effects should stack.
	*/
	/* Effects grouped by their instigator. */
	TMap<UObject*, TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>>> InstigatorEffects;
	TMap<UObject*, TMap<UClass*, FGAGameEffectHandle>> InstigatorEffectHandles;

	/* Effects grouped by their targer */
	TMap<FGAGameEffectHandle, TSharedPtr<FGAGameEffect>> TargetEffects;

	/* Add Handle for instanced effects ? */
	/* Keeps effects instanced per instigator */
	TMap<UObject*, class UGAEffectInstanced*> InstigatorInstancedEffects;

	/* Keeps effects instanced per target actor. */
	TArray<class UGAEffectInstanced*> TargetInstancedEffects;
public:
	FGAGameEffectContainer();

	void ApplyEffect(const FGAGameEffect& EffectIn, const FGAGameEffectHandle& HandleIn);
	void RemoveEffect(FGAGameEffectHandle& HandleIn);

	void ApplyEffectInstance(class UGAEffectInstanced* EffectIn);

	//modifiers
	void ApplyModifier(const FGAGameEffectModifier& ModifierIn, const FGAGameEffect& EffectIn);
	FGAGameModifierStack GetQualifableMods(const FGAGameEffect& EffectIn
		, const FGACalculationContext& Context);
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
protected:
	void InternalApplyModifiers(const FGAGameEffectHandle& HandleIn);
	void InternalCheckEffectStacking(const FGAGameEffectHandle& HandleIn);
	void InternalApplyEffectByAggregation(const FGAGameEffectHandle& HandleIn);
	void InternalApplyEffect(const FGAGameEffectHandle& HandleIn);
	void InternalExtendEffectDuration(const FGAGameEffectHandle& HandleIn, const FGAGameEffectHandle& ExtendingHandleIn);
	
};