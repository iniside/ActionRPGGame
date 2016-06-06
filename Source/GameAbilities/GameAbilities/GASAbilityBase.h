#pragma once
#include "IGIPawn.h"
#include "GASGlobals.h"
#include "GAGameEffect.h"
#include "GameplayTasksComponent.h"
#include "GameplayTask.h"
#include "GameplayTaskOwnerInterface.h"
#include "GAAttributesBase.h"
#include "IGAAttributes.h"
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

	Moar TODO: (idk, if the above is still relevelant).
	1. Add simple tracing directly inside ability. Aside from targeting tasks.
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

	Abilities are using state machine, combined with effects, to perform actions, like
	casting, channeling etc. 
	It needs to be better explained on how each state makes use of effect parameters (Duration, Period).
*/
DECLARE_MULTICAST_DELEGATE(FGASSimpleAbilityDynamicDelegate);

UCLASS(BlueprintType, Blueprintable)
class GAMEABILITIES_API UGASAbilityBase : public UObject, public IIGIPawn, public IGameplayTaskOwnerInterface, public IIGAAttributes
{
	GENERATED_BODY()
public:
	/* By default all abilities are considered to be replicated. */
	UPROPERTY(EditAnywhere, Category = "Replication")
		bool bReplicate;

	bool bIsNameStable;

	bool bIsAbilityExecuting;
	bool bIsOnCooldown;

	TWeakObjectPtr<class UGASAbilitiesComponent> OwningComp;
	//possibly map TMap<FName, Task*> ?
	UPROPERTY()
		TArray<class UGameplayTask*> ActiveTasks;

	/*
		Delegate is used to confirm ability execution.
		After confirming ability will proceed to activation state and either casts instatly or start
		casting effect.
	*/
	FGASSimpleAbilityDynamicDelegate OnConfirmDelegate;
	/*
		Delegate which is called after ability is confirmed and then cast time ended.
	*/
	FGASSimpleAbilityDynamicDelegate OnConfirmCastingEndedDelegate;
	FSimpleDelegate ConfirmDelegate;

	/* Attributes specific to ability. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Attributes")
		UGAAttributesBase* Attributes;

	UPROPERTY()
	class UWorld* World; 
	/*
		Replicated to everyone because we will need it, to determine cosmetic stuff on clients.
	*/
	/*
	*/
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "Default")
		APawn* POwner;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Default")
		APlayerController* PCOwner;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Default")
		class AAIController* AICOwner;
	UPROPERTY(BlueprintReadOnly, Category = "Default")
		class UGASAbilitiesComponent* AbilityComponent;
	/* 
		Physical reprsentation of ability in game world. It might be sword, gun, or character.
		What differs it from pawn or controller is that Avatar is actually used by ability to perform actions.

		It will need some common interfaces for getting data out.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Default")
		class AActor* AvatarActor;

	UPROPERTY(BlueprintReadOnly, Category = "Default")
		UCameraComponent* OwnerCamera;
	/*
		TSubclassOf<> could be better, for runtime customization (ie, different cues for single
		ability). On other hand, instanced is easier to customize in editor. 
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Cue")
	class UGASAbilityCue* Cue;
	/*
		Array ?
		Should it be spawned automatically ?
		Where should it spawn then ? (in world, location, rotation etc ?).
		It should be spawned only on client side.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cue")
		TSubclassOf<class AGASCueActor> ActorCueClass;

	UPROPERTY()
	class AGASCueActor* ActorCue;
	/*
		Tags applied to instigator of this ability, for duration of cooldown.
		Duration of this effect equals cooldown of ability.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAEffectSpec CooldownEffect;
	/*
		These attributes will be reduced by specified amount when ability is activated.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAEffectSpec AttributeCost;
	/*
		Tags applied to the time of activation ability.
		Only applies to abilities, which are not instant (for now).
		Though even instant abilities have animation time,
		they probabaly never apply activation tags, since
		main usecase for those tags is to make other abilities
		being able to interrupt them.
		All abilities with casting/channeling time use it.

		Add Periodic Effect ? (For abilities with period).
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAEffectSpec ActivationEffect;
	/*
		Probably need that
		as well as separate spawned actor, which takes care on it's own for
		abilities effect (like lighting strike from heaves into certain hit point);
	*/
	/* What/Where ability hits landed. Replicated back to client ? */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_AbilityHits)
		FGASAbilityHitArray AbilityHits;
	UFUNCTION()
		void OnRep_AbilityHits();
public: //because I'm to lazy to write all those friend states..
	UPROPERTY()
		FGAGameEffectHandle CooldownHandle;
	UPROPERTY()
		FGAGameEffectHandle AttributeCostHandle;
	UPROPERTY()
		FGAGameEffectHandle ActivationEffectHandle;
	UPROPERTY()
		float LastActivationTime;
	UPROPERTY()
		float LastCooldownTime;
	UFUNCTION()
		void OnCooldownEffectExpired();
	UFUNCTION()
		void OnActivationEffectExpired();
	UFUNCTION()
		void OnActivationEffectRemoved();
	UFUNCTION()
		void OnActivationEffectPeriod();

	UFUNCTION()
		void OnRep_CooldownStarted();
	UFUNCTION()
		void OnRep_CooldownExpired();
	UFUNCTION()
		void OnRep_AbilityActivationStarted();
	UFUNCTION()
		void OnRep_AbilityActivated();
	UFUNCTION()
		void OnRep_AbilityPeriod();
	/* 
		Do any needed client side initialization here. 
		Note that anything created trough this function will exist ONLY on client, this function
		was called.
	*/
	UFUNCTION()
		virtual void OnRep_InitAbility();

	/* Replication counters for above events. */
	UPROPERTY(ReplicatedUsing = OnRep_CooldownStarted)
		uint8 CooldownStartedCounter;
	UPROPERTY(ReplicatedUsing = OnRep_CooldownExpired)
		uint8 CooldownEffectExpiredCounter;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityActivationStarted)
		uint8 AbilityActivationStartedCounter;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityActivated)
		uint8 AbilityActivatedCounter;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityPeriod)
		uint8 AbilityPeriodCounter;
	UPROPERTY(ReplicatedUsing = OnRep_InitAbility)
		uint8 InitAbilityCounter;

public:
	UGASAbilityBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		void PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed = 1);

	virtual void InitAbility();
	/*
		Called when ability received input.
		One ability can receive multiple input calls, how those will be handled 
		is up to ability.
	*/
	void OnNativeInputPressed();
	/*
		Called when ability receive input press. Does not start ability execution automatically, so it
		is safe to make any pre execution preparations of ability here.

		To start ability execution you must call ExecuteAbility.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnInputPressed();

	void OnNativeInputReleased();
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnInputReleased();


	virtual void NativeOnBeginAbilityActivation();

	void NativeOnAbilityConfirmed();

	/*
		In blueprint, call this function to trigger event of the same name,
		after ability is ready to be executed (like after targeting is done, input is confirmed,
		or simply after receiving first input press.

		This function will trigger selected Activation State, which then will trigger Event OnAbilityExecuted.
		In C++ it will trigger OnAbilityExecutedNative().

		ActiveState will always be called first, so it is possible to bypass entire state stage if ActivationState will
		go straight away for event call.
	*/
	UFUNCTION(BlueprintCallable, Category = "Abilities")
		void StartActivation();
	/*
		Event triggered at the begining of ability activation.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnBeginAbilityActivation();

	/*
		Event triggered when Activation effect expire (naturally finish).
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityActivated();

	/* Event called when ability finishes it's execution. Called AFTER OnAbilityExecuted. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnStopedActivation();

	UFUNCTION(BlueprintImplementableEvent, Category = "Abilities")
		void OnAbilityPeriod();

	/*
		Stop effect activation and remove activation effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		void StopActivation();
	void NativeStopActivation();

	bool IsWaitingForConfirm();
	void ConfirmAbility();

	bool CanUseAbility();
	bool CanReleaseAbility();
	
	float GetCooldownTime() const;
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Cooldown Time"), Category = "Game Abilities")
		float BP_GetCooldownTime() const;

	float GetActivationTime() const;
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Activation Time"), Category = "Game Abilities")
		float BP_GetActivationTime() const;

	/** GameplayTaskOwnerInterface - Begin */
	virtual UGameplayTasksComponent* GetGameplayTasksComponent(const UGameplayTask& Task) const override;
	/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
	/** Get owner of a task or default one when task is null */
	virtual AActor* GetGameplayTaskOwner(const UGameplayTask* Task) const override;

	/** Get "body" of task's owner / default, having location in world (e.g. Owner = AIController, Avatar = Pawn) */
	virtual AActor* GetGameplayTaskAvatar(const UGameplayTask* Task) const override;

	/** Get default priority for running a task */
	virtual uint8 GetGameplayTaskDefaultPriority() const {	return 1; };

	/** Notify called after GameplayTask finishes initialization (not active yet) */
	virtual void OnGameplayTaskInitialized(UGameplayTask& Task) override;

	/** Notify called after GameplayTask changes state to Active (initial activation or resuming) */
	virtual void OnGameplayTaskActivated(UGameplayTask& Task) override;

	/** Notify called after GameplayTask changes state from Active (finishing or pausing) */
	virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
	/** GameplayTaskOwnerInterface - end */
	
	/** IIGAAttributes Begin */
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual class UGAAttributeComponent* GetAttributeComponent() override;
	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;
	/* IIGAAttributes Begin **/
	UFUNCTION(BlueprintPure, Category = "Game Attributes")
		virtual float GetAttributeVal(FGAAttribute AttributeIn) const;

	/*
		These functions are specific to ability, and should be used
		over static functions when applying effect from ability.
	*/
	/*
		Due to limitations of blueprints, we need to pass copy of handle (if pass by ref it will
		show  up as return in blueprint, and if as const ref we won't be able to modify handle),
		and then return as copy.
	*/
	/* Effect Helpers */
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		FGAGameEffectHandle ApplyEffectToActor(const FGAEffectSpec& SpecIn,
			FGAGameEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
			UObject* Causer);

	void RemoveEffectFromActor(FGAGameEffectHandle& HandleIn, class AActor* TargetIn);

	/*
		Cues handling
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Abilities | Cues")
		void ActivateActorCue(FVector Location);
	UFUNCTION(NetMulticast, Reliable, Category = "Game Abilities | Cues")
		void MulticastActivateActorCue(FVector Location);
	void MulticastActivateActorCue_Implementation(FVector Location);

protected:
	FGAEffectContext MakeActorContext(class AActor* Target, class APawn* Instigator, UObject* Causer);
	FGAEffectContext MakeHitContext(const FHitResult& Target, class APawn* Instigator, UObject* Causer);
	void AddTagsToEffect(FGAGameEffect* EffectIn);
public:
	bool ApplyCooldownEffect();
	bool ApplyActivationEffect();
	bool ApplyAttributeCost();

	bool CheckCooldown();
	bool CheckExecuting();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Cooldown"), Category = "Game Abilities")
		void BP_ApplyCooldown();

	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		float GetCurrentActivationTime();

	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		float CalculateAnimationSpeed(UAnimMontage* MontageIn);

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
