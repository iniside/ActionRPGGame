#pragma once
#include "GameplayTagContainer.h"
#include "../GAGlobalTypes.h"
#include "GAGameEffect.h"

#include "GameplayTaskOwnerInterface.h"
#include "GAEffectExtension.generated.h"
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
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class ABILITYFRAMEWORK_API UGAEffectExtension : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Context")
		FGAEffectContext Context;
	UPROPERTY()
		class UAFEffectsComponent* OwningComponent;
	UPROPERTY()
		class AActor* Avatar;

	UPROPERTY()
		TSet<class UAFEffectTask*> ActiveTasks;

	UPROPERTY()
		TMap<FName, class UAFEffectTask*> Tasks;

public:
	UGAEffectExtension(const FObjectInitializer& ObjectInitializer);
	void SetParameters(const FGAEffectContext& ContextIn);
	void BeginEffect();

	/*
		This event is always executed once upon application.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectApplied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExecuted();
	/*
		Event executed when effet naturally expires.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExpired();
	/*
		Event executed when this effect is removed by force.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectRemoved();

	void NativeOnEffectApplied();
	void NativeOnEffectExecuted();
	void NativeOnEffectExpired();
	void NativeOnEffectRemoved();

	virtual UWorld* GetWorld() const override;
};
