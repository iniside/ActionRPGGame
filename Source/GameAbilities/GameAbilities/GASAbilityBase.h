#pragma once
#include "GameplayTasksComponent.h"
#include "GameplayTask.h"
#include "GameplayTaskOwnerInterface.h"
#include "GASAbilityBase.generated.h"
/*
	TODO::
	1. Add virtual functions, for default behaviours inside ability.
	And figure out some clever names for them. Functions like:
	ExecuteAbility()
	Possibly default input functions.
	The interface for it, is not yet fully determined. 
	It would be best if the functions and names of them, made sense.. For both AI and player
	pawns.
	2. Add linked abilities. I'm not 100% sure if it really should be default functionality,
	but the idea is that after first ability is executed, it will automatically set next, ability
	for execution in chain, and so on, until last ability. Last ability will, simply back to first.
	This can be implement as very simple linked list (though I'm not sure about nice BP workflow),
	at ability level, OR it can be implemented at component level as queue.
	But, from game design perspective implementation on ability level makes more sense.
*/
/*
	Base class for abilities. It will only implement few generic virtual functions, needed by all
	abilities.

	Ability is what actor can do, not nessesarly, how actor will go about doing it.
	For example we can have jump ability, which is nothing else, than object encapsulating,
	access jump function inside Movement Component.
	
	This way you can give actors, certain abilities, which they can then used based on
	some critera (for example defined in Blackboard and Behaviour tree), and if it is
	possible for actor to perform ability, he will do it (though, one must be careful to
	give abilities, which can be performed by actors, an ordinary actor, can't jump).

	More complicated abilities, can be actions in their own right. Like spells.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEABILITIES_API UGASAbilityBase : public UObject, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
public:
	UPROPERTY()
		TArray<class UGameplayTask*> ActiveTasks;
public:
	UGASAbilityBase(const FObjectInitializer& ObjectInitializer);

	/* 
		Called when ability received input.
		One ability can receive multiple input calls, how those will be handled 
		is up to ability.
	*/
	virtual void OnAbilityExecutedNative();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityExecuted();

	virtual void OnAbilityCancelNative();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityCancel();

	/** GameplayTaskOwnerInterface - Begin */
	virtual void OnTaskInitialized(UGameplayTask& Task) override;
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
	virtual void OnTaskActivated(UGameplayTask& Task) override;
	/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
	virtual void OnTaskDeactivated(UGameplayTask& Task) override;
	virtual AActor* GetOwnerActor(const UGameplayTask* Task) const override;
	virtual AActor* GetAvatarActor(const UGameplayTask* Task) const override;
	virtual uint8 GetDefaultPriority() const override { return 0; }

	/** GameplayTaskOwnerInterface - end */

};
