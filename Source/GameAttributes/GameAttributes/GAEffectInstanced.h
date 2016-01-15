#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"
#include "GAEffectInstanced.generated.h"
/*
	Instanced effect with active event graph, where you can bind events
	on application.

	One instance per ability is applied per actor. (ie, one ability can apply only one instanced
	effect to each actor). 
	So this is not suitable for effects which should have ability to be applied multiple times the same
	actor to stack in intensity and still have different durations.

	When the same type of instanced effect from the same instigator will be applied to the same actor
	old effect will be terminated and new one will be applied.
	Or, old one will be refreshed (reset duration, reinitialize etc, but not destroyed).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffectInstanced : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Effect Info")
		float Duration;

	UPROPERTY(EditAnywhere, Category = "Effect Info")
		EGAEffectAggregation EffectAggregation;

	UPROPERTY(BlueprintReadOnly, Category = "Context")
		FGAEffectContext Context;

	/*
		Effects owned by this instance, they will be removed when this instance is removed.
	*/
	UPROPERTY()
		TArray<FGAGameEffectHandle> OwnedEffects;

public:
	UGAEffectInstanced(const FObjectInitializer& ObjectInitializer);

	void Initialize(const FGAEffectContext& ContextIn);

	UFUNCTION(BlueprintCallable, Category = "Game Attributes | Effects")
	FGAGameEffectHandle ApplyEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn);
	/*
		This event is always executed once upon application.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceApplied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceContinious();

	void NativeOnEffectExpired();
	void NativeOnEffectRemoved();

	/*
		Event executed when this effect is removed by force.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceRemoved();
	/*
		Event executed when effet naturally expires.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceExpired();

protected:
	void InternalEffectEnded();
};
