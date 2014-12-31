#pragma once
#include "IGTTrace.h"
#include "GASAbility.generated.h"

UENUM(BlueprintType)
enum class EGASTraceAbility : uint8
{
	Pawn,
	Avatar,

	Invalid,
};

/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMEABILITIES_API AGASAbility : public AActor, public IIGTTrace
{
	GENERATED_UCLASS_BODY()
	friend class UGASAbilityStateCasting;
public:
	virtual void Tick(float DeltaSeconds) override;


	virtual void BeginDestroy() override;
	/*
		Property here or in states ?
		CastTime, ChannelTime, period time (Ability pulse, every X seconds while active or while
		pressed)
	*/
	UPROPERTY()
		float MaxCastTime;

	UPROPERTY()
		float CooldownTime;

	virtual void BeginPlay() override;
private:
	float CurrentCooldownTime;
	float CurrentCastTime;
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
	/**
	*	State used when this ability is active
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* ActiveState;
	/**
	*	State used when this ability has recived input.
	*/
	UPROPERTY(EditAnywhere, Instanced, meta = (MetaClass = "GASAbilityStatePreparation"), Category = "Ability State")
	class UGASAbilityState* PreparationState;
	/**
	*	State used when ability is on cooldown.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* CooldownState;
	/**
	 *	State used when ability is activated. After preperation state.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Targeting")
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
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Target Data")
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

	UPROPERTY()
		APawn* PawnOwner;
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
