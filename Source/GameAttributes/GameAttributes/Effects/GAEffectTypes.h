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
	Intensity, //will add magnitued to existing effect.
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
struct GAMEATTRIBUTES_API FGAEffectAttributeSpec
{
	GENERATED_USTRUCT_BODY()
public:
	/*
	To who we apply attribute changes.
	*/
	UPROPERTY()
		FGAttributeContext AttributeContext;
	/*
	What attributes we will be modifying.
	*/
	UPROPERTY()
		FGAAttributeSpec AttributeSpec;
};
USTRUCT(BlueprintType)
struct GAMEATTRIBUTES_API FGAEffectSpec
{
	GENERATED_USTRUCT_BODY()
public:
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
	/*
		This is tricky one. We can use it effect, to apply it directly to something (like damage or healing)
		or we can use it as base of calculations.

		The intended use though, is that you precalculate it in something like ability/weapon, before applying,
		and then effect will just either just apply it directly, or overtime.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		FGameplayTag EffectTag;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		float Duration;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		int32 PeriodCount;

	UPROPERTY()
		TArray<FGAAttributeSpec> AttributeSpec;

	FGAEffectPolicy EffectPolicy;

	class UGAEffect* CreateEffect(); //template
};
