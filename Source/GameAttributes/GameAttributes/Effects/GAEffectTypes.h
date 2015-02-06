#pragma once
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "../GAGlobalTypes.h"
#include "GAEffectTypes.generated.h"


UENUM()
enum class EGAEffectType : uint8
{
	Instant,
	Periodic,
	Duration,
	Infinite
};

UENUM()
enum class EGAEffectStacking : uint8
{
	Replace, //will end previous effect, and replace with new one.
	Restart, //will restart existing effect
	Duration, //will add duration to existing effect
	Intensity, //will add magnitude to existing effect.
	Add, //will simply add new effect

	Invalid
};

UENUM()
enum class EGAEffectModPolicy : uint8
{
	Permament, //mod appilies permament changes to attribute, we will never restore value after we expire.
	//be careful with it!
	Restore //if/when we expire we will restore attribute by mod we modified it while
	//we have been active.
};

USTRUCT(BlueprintType)
struct FGAEffectPolicy
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectModPolicy ModPolicy;
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectType EffectType;
	UPROPERTY(EditAnywhere, Category = "Policy")
		EGAEffectStacking EffectStacking;
	UPROPERTY(EditAnywhere, Category = "Policy")
		uint32 bInstanceEffect : 1;
};

USTRUCT(BlueprintType)
struct FGAEffectAttributeSpec
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Spec")
		TArray<FGAAttributeSpec> InitialAttributeSpec;
	UPROPERTY(EditAnywhere, Category = "Spec")
		TArray<FGAAttributeSpec> PeriodicAttributeSpec;
	UPROPERTY(EditAnywhere, Category = "Spec")
		TArray<FGAAttributeSpec> EndAttributeSpec;

	bool IsValid() const
	{
		for (const FGAAttributeSpec& spec : InitialAttributeSpec)
		{
			if (!spec.IsValid())
				return false;
		}
		for (const FGAAttributeSpec& spec : PeriodicAttributeSpec)
		{
			if (!spec.IsValid())
				return false;
		}
		for (const FGAAttributeSpec& spec : EndAttributeSpec)
		{
			if (!spec.IsValid())
				return false;
		}
		return true;
	}

	bool InitialzeSpecs(const FGAttributeContext& Context);
};

USTRUCT(BlueprintType)
struct FGAEffectTags
{
	GENERATED_USTRUCT_BODY()
public:
	/*
		Tags owned by me.
	*/
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGameplayTagContainer MyTags;
	/*
		Tags, that other effect must have, If I'm to modify it.
	*/
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGameplayTagContainer RequiredTags;
};

USTRUCT(BlueprintType)
struct FGAEffectDuration
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeSpec DurationMagnitude;

	UPROPERTY(EditAnywhere, Category = "Spec")
		FGAAttributeSpec PeriodMagnitude;


	bool IsValid() const
	{
		return DurationMagnitude.IsValid() && PeriodMagnitude.IsValid();
	}
	void InitializeDuration(const FGAttributeContext& Context);
};

USTRUCT()
struct FGAEffectAttributeMods
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FGAAttributeModData AttributeDataInitial;
	/*
	This attribute spec will be applied to target on effect period.
	*/
	UPROPERTY()
		FGAAttributeModData AttributeDataPeriodic;
	/*
	This effect spec will be appilied to target when effects ends (for whatever reason).
	*/
	UPROPERTY()
		FGAAttributeModData AttributeDataEnd;

	void SetAttributeData(const FGAEffectAttributeSpec& SpecIn);

	void InitializeAttributeMods(const FGAttributeContext& Context, const FHitResult& Target);
};

USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
	//only used if we want to instance effect.
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		TSubclassOf<class UGAEffect> EffectClass;
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FHitResult Target;
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		UObject* Causer;
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		APawn* Instigator;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
	class UGAEffectComponent* TargetEffectComp;
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
	class UGAEffectComponent* InstigatorEffectComp;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FGameplayTag EffectTag;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FGAEffectTags EffectTags;

	//add exclusion tags ? Maybe ? First get the basics to work as intended.
	UPROPERTY()
		FGAEffectAttributeMods AttributeMods;
	/*
		Amount of periods (Ticks) = DurationMagnitude / PeriodMagnitude.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FGAEffectDuration Duration;

	//UPROPERTY(BlueprintReadOnly, Category = "Spec")
	//	FGAEffectAttributeSpec EffectAttributeSpec;

	FGAEffectPolicy EffectPolicy;

	void ApplyInitialMods();
	void ApplyPeriodicMods();
	void ApplyEndMods();

	float GetDuration();
	float GetPeriod();

	void SetAttributeData(const FGAEffectAttributeSpec& SpecIn);
	bool InitializeEffectSpecs();

	class UGAEffect* CreateEffect(); //template
};
