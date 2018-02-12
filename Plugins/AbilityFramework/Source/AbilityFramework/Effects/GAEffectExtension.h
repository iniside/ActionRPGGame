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
class ABILITYFRAMEWORK_API UGAEffectExtension : public UObject //, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Context")
		FGAEffectContext Context;
	UPROPERTY()
		class UAFEffectsComponent* OwningComponent;
	UPROPERTY()
		class AActor* Avatar;

	TSet<class UGameplayTask*> ActiveTasks;

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

	/** GameplayTaskOwnerInterface - Begin */
	//virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	///** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
	///** Get owner of a task or default one when task is null */
	//virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;

	///** Get "body" of task's owner / default, having location in world (e.g. Owner = AIController, Avatar = Pawn) */
	//virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;

	///** Get default priority for running a task */
	//virtual uint8 GetGameplayTaskDefaultPriority() const { return 1; };

	///** Notify called after GameplayTask finishes initialization (not active yet) */
	//virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;

	///** Notify called after GameplayTask changes state to Active (initial activation or resuming) */
	//virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	///** Notify called after GameplayTask changes state from Active (finishing or pausing) */
	//virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	/** GameplayTaskOwnerInterface - end */

	virtual UWorld* GetWorld() const override;
};
