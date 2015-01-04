#pragma once
#include "IGTTrace.h"
#include "GTTraceBase.h"
#include "GASAbility.generated.h"

UENUM(BlueprintType)
enum class EGASTraceAbility : uint8
{
	Pawn,
	Avatar,

	Invalid,
};
/*
	Minimum info about target of this ability, to which attach effect.
*/
USTRUCT()
struct FGASCueTargetActorInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		AActor* TargetActor;
};

/*
	Quick prototype!
*/
USTRUCT()
struct FGASCueReplication
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int8 Counter;

	UPROPERTY()
		TArray<AActor*> Targets;
};

/*
	This will cover only single hit of ability.

	For AoE effects, spawning actuall effect in center of hit location of
	AoE, would be probabaly be better solution.
*/
USTRUCT(BlueprintType)
struct FGASAbilityHitInfo
{
	GENERATED_USTRUCT_BODY()
public:
	//force replication, if oany of other properties don't change
	int8 Counter;

	UPROPERTY(BlueprintReadOnly)
		FVector Origin;
	UPROPERTY(BlueprintReadOnly)
		FVector HitLocation;
	UPROPERTY(BlueprintReadOnly)
		AActor* HitActor; //need ?
};
/*
	Visual effects, basics:
	1. Visual effects are only specific to ability.
	2. Which means they should be connected to the states of ability.
	3. We can safely assume, that visual effect == particle effect.
	4. If ability spawn for example some kind of actor, this actor can take care of it's own visual effects.
	5. We can also assume that people might want to spawn more complicated effects, which
	would consist of several different componenets (like mesh, particles, decals), What do ?
	Best solution is to spawn actor, which will contain those effects.
	
	So, each state could spawn visual effect actor. This actor would have effects to play, durning this particular 
	state. If state end, this actor is being destroyed.
	Effect actor is replicated, so it will display effect for all relevelant players.

	The hard part is to keep this actor in sync, so effect transforms will be updated on each client.
	If effect is attached to particular location on one player, and that player moves, so should effect.
	For obvious reasons, it would be best to be updated locally. Once actor is replicated,
	each client should take care of transforms.

	In practice we need only effects for three states. Preparation, Casting and Executed (when ability casting is
	finished).

	Note, that final state, where ability is casted (Executed) is bit differentt than previous states,
	because ability can affect other actors in world (targets), and spawned effect must take care of it.

	It might be simple from spawning explosion, to more complicated like beam connected from
	causer to target.

	If ability creates presistent effect (or something like Rain of fire) it is responsibity of this effect 
	to provide visual cues.

	Otherwise there is nothing special in particular regarding this.
*/
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMEABILITIES_API AGASAbility : public AActor, public IIGTTrace
{
	GENERATED_UCLASS_BODY()
	friend class UGASAbilityStateCastingBase;
	friend class UGASAbilityStateCasting;
	friend class UGASAbilityStateCastingCharged;
	friend class UGASAbilityStateChanneled;
	friend class UGASAbilityStateChanneledCharged;
	friend class UGASAbilityStateCooldown;
	friend class UGASAbilityStatePreparation;
	friend class UGASAbilityStatePreparationNoPrep;
	friend class UGASAbilityStatePreparationWaitForInput;
public:
	virtual void Tick(float DeltaSeconds) override;


	virtual void BeginDestroy() override;
	/*
		Property here or in states ?
		CastTime, ChannelTime, period time (Ability pulse, every X seconds while active or while
		pressed)
	*/
protected:
	/**
	 *	Cast time for this ability. How long it will take after pressing input
	 *	to execute ability.
	 *	Set 0 for instant cast.
	 */
	UPROPERTY(EditAnywhere, Category = "Using")
		float CastTime;

	/**
	 *	Used only with channeled state.
	 *	Indicates how often channeled ability will be triggered.
	 */
	UPROPERTY(EditAnywhere, Category = "Using")
		float PeriodLenght;
	/**
	 *	Used only with channeled state.
	 *	Indicates how many time ability will be triggered.
	 */
	UPROPERTY(EditAnywhere, Category = "Using")
		float PeriodCount;

	/**
	 *	Ability cooldown.
	 */
	UPROPERTY(EditAnywhere, Category = "Using")
		float CooldownTime;

	UPROPERTY()
		float CastTimeModifier;

	UPROPERTY(EditAnywhere, Category = "Animation")
		UAnimMontage* CastingMontage; //anim montage or raw animation ?


	/*
		Quick prototype for effect replication - Begin;
	*/
	UPROPERTY(ReplicatedUsing=OnRep_CueRep, RepRetry)
		FGASCueReplication CueReplication;
	UFUNCTION()
		void OnRep_CueRep();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Visual Cues")
		UParticleSystem* ParticleEffect;
	/*
		Quick prototype for effect replication - End;
	*/
	virtual void BeginPlay() override;
private:
	float CurrentCooldownTime;
	float CurrentCastTime;

	/*
		These are small helpers, which will help with spawning cosmetics effects (and ending them).
		technically i would need them for all relevelant states..
		fortunetly it's not that much of data, simple int8 (byte).
		technically even 16 bits would be enough as long as I can increcement it beyond 2 ;).

		Also it could be done usig NetMulticast.
	*/
protected:
	UPROPERTY(ReplicatedUsing=OnRep_CastStarted)
		int8 AbilityCastStarted;
	UFUNCTION()
		void OnRep_CastStarted();

	UPROPERTY(ReplicatedUsing = OnRep_CastEnded)
		int8 AbilityCastEnded;
	UFUNCTION()
		void OnRep_CastEnded();

	UPROPERTY(ReplicatedUsing = OnRep_PreparationStarted)
		int8 PreparationStarted;
	UFUNCTION()
		void OnRep_PreparationStarted();

	UPROPERTY(ReplicatedUsing = OnRep_PreparationEnded)
		int8 PreparationEnd;
	UFUNCTION()
		void OnRep_PreparationEnded();

	UPROPERTY(ReplicatedUsing = OnRep_AbilityHitInfo, BlueprintReadOnly, Category ="Hit Info")
		FGASAbilityHitInfo AbilityHitInfo;
	UFUNCTION()
		void OnRep_AbilityHitInfo();

public:
	void AbilityCastStart();
	void AbilityCastEnd();
	void AbilityPreparationStart();
	void AbilityPreparationEnd();

	void SetAbilityHitInfo(const FVector& Origin, const FVector& HitLocation, AActor* HitTarget);

	/**
	 *	Override to control visual effects when ability casting start.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Cues")
		bool OnAbilityCastStarted();
	/**
	 *	Override to controll visual effects when ability casting is finished.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Cues")
		bool OnAbilityCastEnd();
	/**
	 *	Override to controll visual effects when ability is in preparation state.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Cues")
		bool OnAbilityPreperationStarted();
	/**
	 *	Override to controll visual effects when ability preparation state end.
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Cues")
		bool OnAbilityPreperationEnd();

	/**
	 *	Override to controll visual effects, when ability hits something.
	 *	For example with channeled ability it might be used to controll ray origin/hit location.
	 *	Or for AoE ability it might controll location or AoE visual effect Actor (or whatever
	 *	ability will spawn to indicate current action).
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "Visual Cues")
		bool OnAbilityHit();
public:
	inline float GetCooldownTime() { return CurrentCooldownTime; };

	UPROPERTY(EditAnywhere, Category = "Configuration")
		EGASTraceAbility TraceFrom;

	/**
	 *	Do we require avatar to be present for this ability to work ?
	 */
	UPROPERTY(EditAnywhere, Category = "Configuration")
		bool bRequireAvatar;

	/*
		Maximum range of ability.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Configuration")
		float AbilityRange;
	/*
		Radius of ability.

		This I'm not so sure if I want it here. Not all abilities are AoE,
		but it's useful, for determining sizes of things like targeting helpers.

		More over what if ability is... Wall ? No radial ? Should I also provide
		Rectangle parameters or should I determine lenght from radius 
		and depth, being arbitrary or as separate property ?

		Probabaly best option is to move all of it to Trace action where it belgons
		in first place, and just figure out better way to modify it at runtime.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Configuration")
		float Radius;

	/*
		Field size of ability which searches for targets in rectangle.
	*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Configuration")
		FVector FieldSize;

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		FText AbilityName;

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		UTexture2D* AbilityIcon;

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		FText AbilityDescription;
	/*
		Does this ability have preparation stage. Ie. in AoE ability if you press button to activate it
		you might want to display AoE targetting circle first, and after second input press, activate
		ability to launch it place where targetting circle is.

		If this is false, ability will launch instantly after button press.


		This is probabaly bad idea. What I should do is to add addtional intermediate state. 
		Which would be called preparation state. It would called after Active state
		but before activation state. By default, it would slip right trough it
		and go to activation state. But it could be overriden to perfor specific actions,
		like display targeting helper, and after another input press it would go to activation state.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability")
		bool bHasPreparationStage;

	/**
	 *	State Machine Properties Begin
	 */
	/*
		Add missing states:
		1. Channeled - channeled ability. Have period lenght and max periods = max
		channel time.
		2. Charged - you charge ability two variations:
		2a). Just like casted but you must keep pressing button to fully charge ability.
		2b). with thresholds, at each charge threshold, ability can become stronge
		or do something differently.
		3b) channeled charged. Exactly the same as channeled, except you have to keep
		pressing button.
		4. Instant cast = casted with 0 cast time. Though separate state is more
		self explaning.

		Two Preparation states NoPreparation and WaitforInput.
	 */
	/*
		Add  ability to modify timers by outside objects (cast time, period time, etc).
	 */
	/**
	*	State used when this ability is active
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* ActiveState;
	/**
	*	State used when this ability has recived input.
	*/
	UPROPERTY(EditAnywhere, Instanced, meta = (MetaClass = "UGASAbilityStatePreparation"), Category = "Ability State")
	class UGASAbilityState* PreparationState;
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
	
	/**
	 *	State Machine Properties End
	 */

	/**
	 *	Default Targeting action for this ability. Targeting action will gather information about target, 
	 *	which can be later utilized in blueprint.
	 *	They might also display targeting helpers (like splines, circles, spheres).
	 * 
	 *	Those actions are executed independetly on server and client.
	 *	Which means they might be out of sync if client suffers immense lag or is trying to cheat.
	 *	As server have it's own real version of this object running, and does not replicate it for client.
	 *	Client version is mainly used to help with targeting by displaying various targeting helpers.
	 *	Play animations, particle effects, depends on how you want to indicate that ability
	 *	is currently in "targeting" mode and player must press button again to execute it.
	 *	This possibly could be done in state machine, but action which are point hit
	 *	don't really need such things.
	 *	Also while useful it is certainly not critical enough to waste bandwith and CPU on replicating 
	 *	this object to clients.
	 */

	UPROPERTY(EditAnywhere, Instanced, Category = "Targeting")
	class UGTTraceBase* Targeting;
	/**
	 *	Add here any ability mods, which should alwyas be precent on this ability.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Default Mods")
		TArray<class UGASAbilityMods*> AbilityMods;
	
	/**
	 *	These mods can be added on runtime. You can also create predefined list of mods
	 *	which can be activated (constructed) or deactivated (destroyed).
	 */
	UPROPERTY(EditAnywhere, Category = "Defaul Mods Classes")
		TArray<TSubclassOf<class UGASAbilityMods> > AbilityModsClasses;


	/**
	 *	Temporary. Will probabaly need custom struct for it.
	 *
	 *	Contains list of current targets for this ability. If there is one, there can be more ;).
	 *	the list will be prefiltered, by filters specified by designer upon selection proper TargetingAction.
	 *	Replicated because we will need information about target(s) on client, in order
	 *  to properly handle visual effects.
	 */
	UPROPERTY(BlueprintReadOnly, Replicated, RepRetry, Category = "Target Data")
		TArray<FHitResult> TargetData;

	/*
		This will be useful for tracing from player eyes. What about AI ?
		
		I see two options:
		1. I add somethig generic here like Actor* and you will to work out chain somehow, to get
		useful data to trace from. Doesn't really improve situation.
		2. Extend from this class and add you game specific AI information/tracing here. Which is better
		as I have no idea what kind of AI you might want or if you want to have any at all.

	*/
protected:
	UPROPERTY()
		APlayerController* PCOwner;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Actors")
		APawn* PawnOwner;

	/*
		So we have access to skeletal mesh.
	*/
	UPROPERTY()
		ACharacter* CharOwner;
	/*
		Actor which is directly required, for this ability to work.
		It might be a weapon, player controller or character.

		I assume for most cases it will be weapon or something similiar since character is
		covered by PawnOwner, and PlayerController by PCOwner.

		It might be useful for retriving socket location to start tracing from.
		Mainly for cosmetic purposes. For example to spawn particle effects
		at location.
	*/
	UPROPERTY()
		AActor* AvatarActor;

	class IIGTSocket* iSocket;

	void ExecuteAbility();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerExecuteAbility();

public:
	inline void SetPlayerController(APlayerController* PCOwnerIn){ PCOwner = PCOwnerIn; }
	void SetPawnOwner(APawn* PawnOwnerIn);
	inline void SetAvatarActor(AActor* ActorIn) { AvatarActor = ActorIn; };
public:

	/** IIGTTrace Begin */
	virtual FVector GetSocketLocation(FName SocketNameIn);// { return FVector::ZeroVector; };

	virtual APawn* GetPawn() override { return PawnOwner; };

	virtual APlayerController* GetPC() override { return PCOwner; };

	virtual FVector GetLocation() override { return PawnOwner->GetActorLocation(); };

	virtual void SetTargetData(const TArray<FHitResult>& DataIn);
	virtual TArray<FHitResult>& GetTargetData() override { return TargetData; };

	virtual void SetHitLocation(const FVector& Origin, const FVector& HitLocation, AActor* HitActor) override;
	/** IIGTTrace End */

	/**
	 *	State Machine Function Begin
	 */
	void GotoState(class UGASAbilityState* NextState);

	inline void GotoActiveState()
	{
		GotoState(ActiveState);
	};
	/**
	 *	State Machine Function End
	 */

	/**
	 *	Hook it up to input.
	 */
	
	void InputPressed();
	void InputReleased();

	void ActivateAbility();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerActivateAbility();

	void DeactivateAbility();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDeactivateAbility();

	void RunPreparationActions();

	/**
	 *	Blueprint Events
	 */
	/**
	 *	Called when ability is Activated (ie. casting finished).
	 *	Make it overridable in blueprint ? Or just use it to start event graph ?
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Game Abilities")
		void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Game Abilities")
		void OnAbilityDeactivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownEnded();
};
