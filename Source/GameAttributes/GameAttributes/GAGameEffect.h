#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
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
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere)
		float Value;
	UPROPERTY(EditAnywhere)
		EGAAttributeMod ModType;

	UPROPERTY(EditAnywhere)
		EGAModifierDirection Direction;

	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAGameEffectInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		float Value;
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		EGAAttributeChangeType ChangeType;
};

UCLASS(Blueprintable, BlueprintType)
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

	/* 
		Modifiers applied upon application of effect. They are treated as instant effect
		as never will be applied for duration.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAGameEffectInfo> OnAppliedInfo;

	/*
		Modifiers applied on effect period.
	*/
	UPROPERTY(EditAnywhere, Category = "Attribute Modifiers")
		TArray<FGAGameEffectInfo> OnPeriodInfo;

	/* For now just test instant application of direct value */
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		float Value;
	UPROPERTY(EditAnywhere, Category = "Instant Spec")
		EGAAttributeChangeType ChangeType;

	UPROPERTY(EditAnywhere, Category = "Calculation Type")
		TSubclassOf<class UGACalculation> CalculationType;

	UPROPERTY(EditAnywhere, Category = "Modifiers")
		FGAGameEffectModifier Modifiers;

	/* Tags I own and I don't apply */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer OwnedTags;

	/* If Target have these tags I will not be applied to it. */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DenyTags;
public:
	UGAGameEffectSpec();
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
/* Final calculcated mod from effect, which can be modified by Calculation object. */
struct FGAEffectMod
{
	FGAAttribute Attribute;
	float Value;
	EGAAttributeChangeType ChangeType;
	
	/* 
		Spec from which this mod has been derived. 
		Used to do not copy to much heavy data around.
	*/
	class UGAGameEffectSpec* EffectSpec;

	FGAEffectMod()
		: Attribute(NAME_None),
		Value(0),
		ChangeType(EGAAttributeChangeType::Invalid)
	{};

	FGAEffectMod(const FGAAttribute& AttributeIn, float ValueIn, 
		EGAAttributeChangeType ChangeTypeIn, class UGAGameEffectSpec* EffectSpecIn)
		: Attribute(AttributeIn),
		Value(ValueIn),
		ChangeType(ChangeTypeIn),
		EffectSpec(EffectSpecIn)
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
	class UGACalculation* Calculation;
	
	/* 
		Calculated mods ready to be applied. 
		These are perlimenarly calculcated mods, 
		which can be furhter modified by Calculation object.


	*/

	FGAEffectContext Context;
	FGameplayTagContainer OwnedTags;

	struct FGAGameEffectHandle* Handle;

private:
	FTimerHandle PeriodTimerHandle;

public:
	void SetContext(const FGAEffectContext& ContextIn);
	void InitializePeriodic();
	TArray<FGAEffectMod> GetOnAppliedMods();

	class UGAAttributeComponent* GetInstigatorComp() { return Context.InstigatorComp.Get(); }
	class UGAAttributeComponent* GetTargetComp() { return Context.TargetComp.Get(); }

	void OnPeriod(); //internal timer - called by
	void OnExpired() {}; //internal timer
	void OnRemoved() {}; //internal timer

	class UGACalculation* GetCalculation()
	{
		return GameEffect->CalculationType.GetDefaultObject();
	}
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
	TArray<FGAEffectMod> GetMods(TArray<FGAGameEffectInfo>& ModsInfoIn);
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

	bool IsValid()
	{
		return Handle != INDEX_NONE && EffectPtr.IsValid();
	}

	inline int32 GetHandle() const { return Handle; }

	inline FGAGameEffect GetEffect() { return *EffectPtr.Get(); }

	inline FGAGameEffect& GetEffectRef() { return EffectPtr.ToSharedRef().Get(); }

	inline TSharedPtr<FGAGameEffect> GetEffectPtr() { return EffectPtr; };

	inline void SetContext(const FGAEffectContext& ContextIn) { EffectPtr->SetContext(ContextIn); }

	inline FGAEffectContext& GetContext() { return EffectPtr->Context; }

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
public:
	FGAGameEffectContainer();

	void ApplyEffect(const FGAGameEffect& EffectIn, const FGAGameEffectHandle& HandleIn);
	void ExecuteEffect(FGAGameEffect& EffectIn, EGAModifierApplication ModAppType);
	void ExecutePeriodicEffect(FGAGameEffectHandle HandleIn);

	//modifiers
	void ApplyModifier(const FGAGameEffectModifier& ModifierIn);
	FGAGameModifierStack GetQualifableMods(const FGAGameEffect& EffectIn
		, const FGACalculationContext& Context);
	void ApplyEffectsFromMods() {};
	void DoesQualify() {};
};