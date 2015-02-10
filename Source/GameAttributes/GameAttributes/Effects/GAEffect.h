#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAEffect.generated.h"
/*
	1. Provie custom conditional execution paths (if something, then somethingElse, OR this AND NOT that)
	While something, DO this. 
	It will not work for WHEN target do something. Since we have no way for listening to events, within functions.
	We can only check for conditions upon application, or while effect tick (like periodic damage).

	2. Provide custom magnitude calculation option ?
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffect : public UObject
{
	GENERATED_BODY()
public:
	UGAEffect(const FObjectInitializer& ObjectInitializer);

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
