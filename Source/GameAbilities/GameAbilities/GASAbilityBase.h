#pragma once
#include "IGIPawn.h"
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
DECLARE_MULTICAST_DELEGATE(FGASSimpleAbilityDynamicDelegate);

UCLASS(BlueprintType, Blueprintable)
class GAMEABILITIES_API UGASAbilityBase : public UObject, public IIGIPawn, public IGameplayTaskOwnerInterface
{
	GENERATED_BODY()
public:
	/* By default all abilities are considered to be replicated. */
	UPROPERTY(EditAnywhere, Category = "Replication")
		bool bReplicate;

	bool bIsNameStable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
		float Cooldown;

	bool bIsAbilityExecuting;
	bool bIsOnCooldown;

	TWeakObjectPtr<class UGASAbilitiesComponent> OwningComp;

	UPROPERTY()
		TArray<class UGameplayTask*> ActiveTasks;

	FGASSimpleAbilityDynamicDelegate OnConfirmDelegate;
	FSimpleDelegate ConfirmDelegate;

	UPROPERTY()
	class UWorld* World; 

	UPROPERTY(BlueprintReadOnly, Category = "Default")
		APawn* POwner;
	UPROPERTY(BlueprintReadOnly, Category = "Default")
		APlayerController* PCOwner;
	UPROPERTY(BlueprintReadOnly, Category = "Default")
		class AAIController* AICOwner;
	/* 
		Actor which either represents this ability in the world, or
		is used as required "prop" for ability to work.

		Ie Ability granted by machine gun, might simply represented by this weapon.
		Ability which is spell equiped by player, might use Staff or Scepter as representation
		needed to cast (ie access to socket at tip of the staff for spawning effects).
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Default")
		AActor* AvatarActor;

	UPROPERTY(BlueprintReadOnly, Category = "Default")
		UCameraComponent* OwnerCamera;
	/**
	*	Default state to which ability will always return upon finishing (either immidielty or after cooldown).
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* DefaultState;
	/**
	*	State used when ability is on cooldown.
	*/
	UPROPERTY(EditAnywhere, Instanced, meta = (MetaClass = "UGASAbilityStateCooldown"), Category = "Ability State")
	class UGASAbilityState* CooldownState;
	/**
	*	State used when ability is activated. After preperation state.
	*/
	UPROPERTY(EditAnywhere, Instanced, meta = (AllowedClasses = "GASAbilityStateCastingBase"), Category = "Ability State")
	class UGASAbilityState* ActivationState;

	UPROPERTY()
	class UGASAbilityState* CurrentState;

protected:
	FTimerHandle CooldownTimerHandle;

public:
	UGASAbilityBase(const FObjectInitializer& ObjectInitializer);
	virtual void InitAbility();
	/* 
		Called when ability received input.
		One ability can receive multiple input calls, how those will be handled 
		is up to ability.
	*/
	void OnNativeInputPressed();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnInputPressed();

	void OnNativeInputReleased();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnInputReleased();


	virtual void OnAbilityExecutedNative();
	/*
		In blueprint, call this function to trigger event of the same name,
		after ability is ready to be executed (like after targeting is done, input is confirmed,
		or simply after receiving first input press.

		This function will trigger selected Activation State, which then will trigger Event OnAbilityExecuted.
		In C++ it will trigger OnAbilityExecutedNative()
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void ExecuteAbility();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityExecuted();
	/*
		Call this to stop ability. Usually should be connected to OnInputReleased Event.
		Though nothing really stops you from connecting it's elsewhere, most of the cases when ability should 
		end will be handled trough states automatically.
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void StopAbility();

	void OnNativeStopAbility();
	/*
		Special Case event, can will be called trough specific ability states, like
		Channeled or Charged.
		What exacty will happen depends on state/ability implementation.
		In case of Channeled state which requires input to pressed to remain active
		it will stop ability channel, go to cooldown and call this event to give
		ability chance to do something.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnStopAbility();

	virtual void OnAbilityCancelNative();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityCancel();

	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		void FinishExecution();
	void NativeFinishExecution();

	bool IsWaitingForConfirm();
	void ConfirmAbility();

	bool CanUseAbility();

	/* State Handling Begin */
	void GotoState(class UGASAbilityState* NextState);
	/* State HAndling End */

	/** GameplayTaskOwnerInterface - Begin */
	virtual void OnTaskInitialized(UGameplayTask& Task) override;
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
	virtual void OnTaskActivated(UGameplayTask& Task) override;
	/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
	virtual void OnTaskDeactivated(UGameplayTask& Task) override;
	virtual AActor* GetOwnerActor(const UGameplayTask* Task) const override;
	virtual AActor* GetAvatarActor(const UGameplayTask* Task) const override;
	virtual uint8 GetDefaultPriority() const override { return 1; }
	/** GameplayTaskOwnerInterface - end */

	/* Replication */
	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return bReplicate;
	}
	void SetNetAddressable();

	/** IIGIPawn */
	virtual APawn* GetGamePawn() { return POwner; }
	virtual ACharacter* GetGameCharacter() { return nullptr; }
	virtual AController* GetGameController() { return nullptr; }
	virtual APlayerController* GetGamePlayerController() { return PCOwner; }
	/* IIGIPawn **/


	virtual class UWorld* GetWorld() const override;
};
