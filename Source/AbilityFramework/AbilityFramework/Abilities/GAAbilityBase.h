#pragma once
#include "IGIPawn.h"
#include "GAGlobals.h"
#include "../Effects/GAGameEffect.h"
#include "GameplayTasksComponent.h"
#include "GameplayTask.h"
#include "GameplayTaskOwnerInterface.h"
#include "../Attributes/GAAttributesBase.h"
#include "IGAAbilities.h"
#include "AFAbilityActivationSpec.h"
#include "GAAbilityBase.generated.h"

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

	2. Add caching for effects, so we don;'t create new ones every time, just reference handle.
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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGASGenericAbilityDelegate);
USTRUCT()
struct FGAAbilityTick : public FTickFunction
{
	GENERATED_USTRUCT_BODY()
	/**  AActor  that is the target of this tick **/
	class UGAAbilityBase* Target;

	/**
	* Abstract function actually execute the tick.
	* @param DeltaTime - frame time to advance, in seconds
	* @param TickType - kind of tick for this frame
	* @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
	* @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completetion of this task until certain child tasks are complete.
	**/
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
	virtual FString DiagnosticMessage() override;
};
template<>
struct TStructOpsTypeTraits<FGAAbilityTick> : public TStructOpsTypeTraitsBase2<FGAAbilityTick>
{
	enum
	{
		WithCopy = false
	};
};
USTRUCT()
struct FGAActiationInfo
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY()
		float TimeStamp;
	UPROPERTY()
		float Duration;
	UPROPERTY()
		float Period;
	UPROPERTY()
		bool bApplyActivationEffect;

	inline void SetActivationInfo(float TimeStampIn, float DurationIn, float PeriodIn,
		bool bApplyActivationEffectIn)
	{
		TimeStamp = TimeStampIn;
		Duration = DurationIn;
		Period = PeriodIn;
		bApplyActivationEffect = bApplyActivationEffectIn;
		//always increment to make sure it is replicated.
		ForceReplication++;
	}

	UPROPERTY()
		int8 ForceReplication;
};

enum EAFAbilityState
{
	Waiting,
	Activating
};

UCLASS(BlueprintType, Blueprintable)
class ABILITYFRAMEWORK_API UGAAbilityBase : public UObject, public IIGIPawn, public IGameplayTaskOwnerInterface, public IIGAAbilities
{
	GENERATED_BODY()
public:
	FGAAbilityTick TickFunction;

	/* By default all abilities are considered to be replicated. */
	UPROPERTY(EditAnywhere, Category = "Replication")
		bool bReplicate;

	bool bIsNameStable;

	//possibly map TMap<FName, Task*> ?
	UPROPERTY()
		TSet<class UGameplayTask*> ActiveTasks;
	/* List of tasks, this ability have. */
	UPROPERTY()
		TMap<FName, class UGAAbilityTask*> AbilityTasks;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "AbilityFramework|Abilities")
		UGAAttributesBase* Attributes;

	UPROPERTY()
	class UWorld* World; 
	/*
		Replicated to everyone because we will need it, to determine cosmetic stuff on clients.
	*/
	/*
	*/
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AbilityFramework|Abilities")
		APawn* POwner;
	UPROPERTY(VisibleAnywhere, Replicated, BlueprintReadOnly, Category = "AbilityFramework|Abilities")
		ACharacter* Character;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AbilityFramework|Abilities")
		APlayerController* PCOwner;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AbilityFramework|Abilities")
		class AAIController* AICOwner;
	UPROPERTY(BlueprintReadOnly, Category = "AbilityFramework|Abilities")
		class UGAAbilitiesComponent* AbilityComponent;

	/* 
		Physical reprsentation of ability in game world. It might be sword, gun, or character.
		What differs it from pawn or controller is that Avatar is actually used by ability to perform actions.

		It will need some common interfaces for getting data out.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "AbilityFramework|Abilities")
		class AActor* AvatarActor;

	UPROPERTY(BlueprintReadOnly, Category = "AbilityFramework|Abilities")
		UCameraComponent* OwnerCamera;

	/*
		Tags applied to instigator of this ability, for duration of cooldown.
		Duration of this effect equals cooldown of ability.
	*/
	UPROPERTY(EditAnywhere, meta=(AllowedClass="AFAbilityCooldownSpec"), Category = "Config")
		FGAEffectProperty CooldownEffect;
	FGAEffectHandle CooldownEffectHandle;
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
	UPROPERTY(EditAnywhere, meta = (AllowedClass = "AFAbilityActivationSpec,AFAbilityPeriodSpec,AFAbilityInfiniteDurationSpec,AFAbilityInfinitePeriodicSpec"), Category = "Config")
		FGAEffectProperty ActivationEffect;
	FGAEffectHandle ActivationEffectHandle;
	/*
		These attributes will be reduced by specified amount when ability is activated.
		Attribute cost from Ability Owner attributes
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAEffectProperty AttributeCost;
	/*
		Attribute cost from ability own attributes
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAEffectProperty AbilityAttributeCost;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTag AbilityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer OwnedTags;
	/*
		These tags are added to owner while ability is activating (or channeled).
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer ActivationAddedTags;
	/*
		These tags must be present on onwer to activate this ability.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer ActivationRequiredTags;
	/*
		If any of these tags is present ability activation is blocked.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer ActivationBlockedTags;

	/*
		This ability can't be activated if any of these tags is on owner.
		Usefull if you want to make sure only certain abilities can be used togather
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer AbilityDenyTags;

	/* 
		Overrides AbilityDenyTags if any of these tags is present,
		ability can be activated.
		Setting it up properly can allow to hve jump ability, during which we can use any ability,
		Pull ability, which deny all other abilities and push which deny other abilities.
		But we can allow Push to be used when Pull is active.
		Ie. Push apply Tag Ability.Push
		We check for deny tag Ability from AbilityDenyTags. True.
		Then We check for tag Ability.Push from AbilityAllowedTags. True.

	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Tags")
		FGameplayTagContainer AbilityAllowedTags;

public: //because I'm to lazy to write all those friend states..
	UPROPERTY()
		float LastActivationTime;
	UPROPERTY()
		float LastCooldownTime;
	virtual void TickAbility(float DeltaSeconds, ELevelTick TickType, FGAAbilityTick& ThisTickFunction);
	UFUNCTION()
		void OnActivationEffectPeriod(const FGAEffectHandle& InHandle);

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
		FGAActiationInfo ActivationInfo;
	UPROPERTY(ReplicatedUsing = OnRep_AbilityPeriod)
		uint8 AbilityPeriodCounter;
	UPROPERTY(ReplicatedUsing = OnRep_InitAbility)
		uint8 InitAbilityCounter;

	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnInputPressedDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnInputReleasedDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnActivateBeginDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnActivationFinishedDelegate;
protected:
	EAFAbilityState AbilityState;

public:
	UGAAbilityBase(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed = 1);

	virtual void InitAbility();
	/*
		Called when ability received input.
		One ability can receive multiple input calls, how those will be handled 
		is up to ability.
	*/
	void OnNativeInputPressed(FGameplayTag ActionName);
	/*
		Called when ability receive input press. Does not start ability execution automatically, so it
		is safe to make any pre execution preparations of ability here.

		To start ability execution you must call ExecuteAbility.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnInputPressed(FGameplayTag ActionName);

	void OnNativeInputReleased(FGameplayTag ActionName);
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnInputReleased(FGameplayTag ActionName);


	virtual void NativeOnBeginAbilityActivation(bool bApplyActivationEffect);

	void NativeOnAbilityConfirmed();

	/*
		Called when ability get message to activate.
		For player it usually means on button press.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivate();

	/*
		Called When activation effect expired.
		In case of instant effects, called right after OnActivate.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivationFinished();

	/*
		Called when ability deactivates.
		For player it usually means in input release.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnDeactivate();
	/*
		In blueprint, call this function to trigger event of the same name,
		after ability is ready to be executed (like after targeting is done, input is confirmed,
		or simply after receiving first input press.

		This function will trigger selected Activation State, which then will trigger Event OnAbilityExecuted.
		In C++ it will trigger OnAbilityExecutedNative().

		ActiveState will always be called first, so it is possible to bypass entire state stage if ActivationState will
		go straight away for event call.

		bApplyActivationEffect - should apply activation effect ?
		if Period or Activation is > 0, it will be overrided to true.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void StartActivation(bool bApplyActivationEffect);


	/* Event called when ability activation has been canceled. */
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivationCancel();

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnPeriod();

	/* Event called when ability finishes it's execution. Called AFTER OnAbilityExecuted. */
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnFinished();

	UFUNCTION()
		void OnCooldownEffectExpired();
	UFUNCTION()
		void NativeOnAbilityActivationFinish(const FGAEffectHandle& InHandle);
	UFUNCTION()
		void NativeOnAbilityActivationCancel();

	/*
		Finishes ability. Use it to finish ability after activation or released input.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void FinishAbility();
	void NativeFinishAbility();
	/*
		Stop effect activation and remove activation effect.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void CancelActivation();
	void NativeCancelActivation();

	bool IsWaitingForConfirm();
	void ConfirmAbility();

	bool CanUseAbility();
	bool CanReleaseAbility();
	
	float GetCurrentActivationTime() const;
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Current Activation Time"), Category = "AbilityFramework|Abilities")
		float BP_GetCurrentActivationTime() const;
	float GetCurrentCooldownTime() const;

	float GetPeriodTime() const;
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Period Time"), Category = "AbilityFramework|Abilities")
		float BP_GetPeriodTime() const;

	float GetCooldownTime() const;
	UFUNCTION(BlueprintPure, meta=(DisplayName = "Get Cooldown Time"), Category = "AbilityFramework|Abilities")
		float BP_GetCooldownTime() const;

	float GetActivationTime() const;
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Get Activation Time"), Category = "AbilityFramework|Abilities")
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
	
	/** IIGAAbilities Begin */
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual class UGAAbilitiesComponent* GetAbilityComp() override;
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Attributes")
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override { return Attributes->GetAttribute(AttributeIn); };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override { Attributes->RemoveBonus(AttributeIn, HandleIn, HandleIn.GetAttributeMod()); };
	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	, FGAEffectProperty& InProperty) override { Attributes->ModifyAttribute(ModIn, HandleIn, InProperty); };
	virtual FGAEffectHandle ApplyEffectToTarget(FGAEffect* EffectIn,
		FGAEffectProperty& InProperty, FGAEffectContext& InContext) override;
	virtual void RemoveTagContainer(const FGameplayTagContainer& TagsIn) override;

	/* IIGAAbilities End **/
	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Abilities|Attributes")
		virtual float GetAttributeVal(FGAAttribute AttributeIn) const;

public: //protected ?
	bool ApplyCooldownEffect();
	bool ApplyActivationEffect(bool bApplyActivationEffect);
	bool ApplyAttributeCost();
	bool ApplyAbilityAttributeCost();
	bool IsOnCooldown();
	bool IsActivating();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Cooldown"), Category = "AbilityFramework|Abilities")
		void BP_ApplyCooldown();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_ApplyAttributeCost();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Ability Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_ApplyAbilityAttributeCost();

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		float GetCurrentActivationTime();

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
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

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void ExecuteAbilityInputPressedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName);
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void ExecuteAbilityInputReleasedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName);

	virtual class UWorld* GetWorld() const override;

	inline void AddAbilityTask(FName InName, class UGAAbilityTask* InTask)
	{
		if (!AbilityTasks.Contains(InName))
		{
			AbilityTasks.Add(InName, InTask);
		}
	}
	inline class UGAAbilityTask* GetAbilityTask(const FName& InName)
	{
		return AbilityTasks.FindRef(InName);
	}

	/* Tracing Helpers Start */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tracing")
		bool LineTraceSingleByChannel(const FVector Start, const FVector End, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit);
	/* Traces location from owner camera position if no camera available traces from eyes */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tracing")
		bool LineTraceSingleByChannelFromCamera(float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
			EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
	/* Traces from ability avatar socket. */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tracing")
		bool LineTraceSingleByChannelFromSocket(FName SocketName, float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
			EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
	/* Make first trace from camera location and then second trace from avatar socket in direction of first trace. */
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tracing")
		bool LineTraceSingleByChannelCorrected(FName SocketName, float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
			EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime);
	/* Tracing Helpers End */
};
