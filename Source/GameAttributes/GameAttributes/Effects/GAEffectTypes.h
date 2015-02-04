#pragma once
#include "GameplayTags.h"
#include "GameplayTagContainer.h"

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

//USTRUCT()
//struct FGESEffectSpec
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	UPROPERTY()
//		TWeakObjectPtr<AActor> Target;
//	UPROPERTY()
//		TWeakObjectPtr<AActor> Causer;
//	UPROPERTY()
//		TWeakObjectPtr<AActor> Instigator;
//
//	UPROPERTY()
//		TWeakObjectPtr<class UGESEffectComponent> TargetComponent;
//	UPROPERTY()
//		TWeakObjectPtr<class UGESEffectComponent> InstigatorComponent;
//
//	UPROPERTY()
//		TWeakObjectPtr<class UGESEffect> Effect;
//
//	FGESEffectSpec();
//
//	FGESEffectSpec(TWeakObjectPtr<AActor> TargetIn, TWeakObjectPtr<AActor> CauserIn, 
//		TWeakObjectPtr<AActor> InstigatorIn);
//};

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
		FGameplayTagContainer MagnitudeTags;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		float Magnitude;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		float Duration;

	UPROPERTY(BlueprintReadOnly, Category = "Spec")
		int32 PeriodCount;

	class UGAEffect* CreateEffect(); //template
};

//USTRUCT(BlueprintType)
//struct GAMEEFFECTSYSTEM_API FGESEffectContext
//{
//	GENERATED_USTRUCT_BODY()
//public:
//	
//};