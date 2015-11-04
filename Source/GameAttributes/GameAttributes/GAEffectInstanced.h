#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
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
	UGAEffectInstanced(const FObjectInitializer& ObjectInitializer);
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
		bool OnEffectApplied(const FGAEffectContext& Context);
	virtual bool OnEffectApplied_Implementation(const FGAEffectContext& Context);

	/*
		This event is only executed is effect, is instanced, and it's executed when 
		effect is first time applied to target.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceApplied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceContinious();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceRemoved();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectInstanceExpired();
};
