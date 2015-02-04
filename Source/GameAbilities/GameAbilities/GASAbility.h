#pragma once
#include "IGIPawn.h"
#include "IGISkeletalMesh.h"
#include "GASAbility.generated.h"

USTRUCT(BlueprintType)
struct GAMEABILITIES_API FGASHitLocation
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int8 HitCounter;
	UPROPERTY(BlueprintReadOnly, Category = "Hit")
		FVector Origin;
	UPROPERTY(BlueprintReadOnly, Category = "Hit")
		FVector HitLocation;
};
/*
	1. Add linked (sequence) abilities. Which means, when user press input, and ability is executed,
	next ability is queued and wait for input, without triggering cost/cooldown, but still
	might trigger cast time. (or probabaly left it as configuration options)
*/
DECLARE_DELEGATE(FGASOnAbilityCasted)

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMEABILITIES_API UGASAbility : public UObject, public FTickableGameObject, public IIGIPawn
	, public IIGISkeletalMesh
{
	GENERATED_BODY()
	friend class UGASAbilityState;
	friend class UGASAbilityStateActive;
	friend class UGASAbilityStateCastingBase;
	friend class UGASAbilityStateCasting;
	friend class UGASAbilityStateCastingCharged;
	friend class UGASAbilityStateChanneled;
	friend class UGASAbilityStateChanneledCharged;
	friend class UGASAbilityStateChanneledLocked;
	friend class UGASAbilityStateChanneledChargedLocked;
	friend class UGASAbilityStateCooldown;
	friend class UGASAbilityStatePreparation;
	friend class UGASAbilityStatePreparationNoPrep;
	friend class UGASAbilityStatePreparationWaitForInput;
	
	friend class UGASTrace;
	friend class UGASTrace_SingleLineTrace;
	friend class UGASTrace_SphereTrace;

	friend class UGASAbilityAction;

	friend class UGASAbilityCue;

	friend class UGASAbilityMods;
public:
	UGASAbility(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, Category = "Replication")
		bool bReplicate;

	bool bIsNameStable;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
		TArray<FHitResult> TargetData;

	float CurrentCastTime;
	float CurrentRechargeTime;
	UPROPERTY(EditAnywhere, Category = "Using Ability")
		float CastTime;
	UPROPERTY(EditAnywhere, Category = "Using Ability")
		float RechargeTime;

	UPROPERTY(EditAnywhere, Category = "Area")
		float Range;
	/*
		Sphere:
		1. X - Radius
		Box:
		1. Just like box.
		Capsule:
		1. X - Radius
		2. Y - Height (total!).
	*/
	UPROPERTY(EditAnywhere, Category = "Area")
		FVector AbilityArea;
	
	/*
		Where to spawn effects for ability or projectile.

		It can be socket on Pawn or on weapon.
	*/
	UPROPERTY(EditAnywhere, Category = "Area")
		FName SpawnSocketName;

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

	UPROPERTY(EditAnywhere, Instanced, Category = "Targeting")
	class UGASTrace* Targeting;
	/*
		Should update, hit location every frame ? Useful for properly displaying
		effects like beams.
	*/
	UPROPERTY(EditAnywhere, Category = "Targeting")
		bool bUpdateHitLocationEveryFrame;

	UPROPERTY()
	class UGASAbilityState* CurrentState;

	UPROPERTY(BlueprintReadOnly)
	class UGASAbilitiesComponent* AbilityComponent;
	/*
		Can be null! Though shouldn't be. This component should be attach to pawn at very least.
	*/
	UPROPERTY(BlueprintReadOnly)
	class APawn* POwner;
	/*
		Can be null!
	*/
	UPROPERTY(BlueprintReadOnly)
	class APlayerController* PCOwner;
	/*
		Can be null!
	*/
//	UPROPERTY(BlueprintReadOnly)
	//class AAIController* AIOwner;

	/*
		Single cast delegate called on server, and client.
		On client it's used to provide information for UI.
	*/
	FGASOnAbilityCasted OnAbilityCastedDel;
protected:
	IIGISkeletalMesh* GISkeletalMesh;
	bool bShouldTick;
	/*
		Intentionally not replicated, since we will initialize once on server, and once on client
		when ability will be replicated.
	*/
	bool bIsInitialized;

	bool bIsOnCooldown;
public:
	void SetTickEnabled(bool ValueIn){ bShouldTick = ValueIn; };

	virtual void Initialize();

	inline bool GetIsInitialized(){ return bIsInitialized; };
	void InputPressed();
	void InputReleased();
	void InputCancel();

	virtual UWorld* GetWorld() const override;
protected:
	void CancelAbility();

	virtual bool CanCommitAbility();
	virtual void CommitResources();

	/*
		Prepare ability, we go from Active State to Preparation State.
	*/
	virtual void PrepareAbility();

	/*
		Commits ability, which means we will go from PreparationState to ActionState
	*/
	virtual bool CommitAbility();

	/*
		We call it from state machine, to actually cast ability.
	*/
	void CastAbility();

	void CastAbilityInterval();

	void GotoState(class UGASAbilityState* NextState);

	inline void GotoActiveState(){ GotoState(ActiveState); };

	inline void GotoCooldownState(){ GotoState(CooldownState); };

	void RunPreparationActions();

	ENetMode GetNetMode();
	/*
		Called when ability is entering preparation state.
		Useful for setup, things like 
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnAbilityPrepare();
	/*
		Called when ability finished casting and will be fired.

		Also fired on each interval for channeled abilities ?
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnAbilityCasted();


	void HitTarget(AActor* HitActor, const FVector& HitLocation, const FHitResult& Hit);
	/*
		Called when something is hit by ability. Can be called multiple times, if
		ability hit multiple actors!
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnAbilityTargetHit(AActor* HitActor, const FVector& HitLocation, const FHitResult& Hit);

	/*
		Please note that object does not tick contanstly.
		Ticking is enabled/disabled on demand.
	*/
	/** FTickableGameObject */
	virtual void Tick(float DeltaTime) override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	/* FTickableGameObject **/

	/*
		Replication
	*/
	//
	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return bReplicate;
	}
	void SetNetAddressable();

	virtual FVector GetCorrectedSocketLocation(FName SocketNameIn) { return FVector::ZeroVector; };
	/*
		Called when ability is commited (started casting). Probabaly best used to start playing
		montage, and attach effects, to pawn mesh, as.

		Best practives:
		1. When you spawn new effects components, always store it in variable, to have
		reference, you can later use. The same goes for actors, and any other objects.
		Yes there is garbage collector, but it doesn't work instantly ;).
		2. This reference can be used to destroy effect immidietly when it's not needed, so
		it won't linger in world.
		3. You can also use this reference to modify any effects/animations over time
		to create more complex effects.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Cosmetic Events")
		void OnAbilityCastingStarted();
	/*
		Called when ability casting is finished. You can use it to cleanup, or spawn additional
		effects, indicating that ability casting is finished.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Cosmetic Events")
		void OnAbilityCastingEnded();
	//////////////////////////////
	////// Rep Notifies
	UPROPERTY(ReplicatedUsing=OnRep_AbilityCastingStarted)
		int8 AbilityCastingStarted;
	UFUNCTION()
		void OnRep_AbilityCastingStarted();
	UPROPERTY(ReplicatedUsing = OnRep_AbilityCastingEnded)
		int8 AbilityCastingEnded;
	UFUNCTION()
		void OnRep_AbilityCastingEnded();

	UPROPERTY(ReplicatedUsing = OnRep_HitLocation)
		FGASHitLocation HitLocation;
	UFUNCTION()
	void OnRep_HitLocation();

	////// Rep Notifies
	//////////////////////////////

	void SetHitLocation(const FVector& OriginIn, const FVector& HitLocationIn);
public:
	/** IIGIPawn */
	virtual APawn* GetGamePawn() { return POwner; }
	virtual ACharacter* GetGameCharacter() { return nullptr; }
	virtual AController* GetGameController() { return nullptr; }
	virtual APlayerController* GetGamePlayerController() { return PCOwner; }
	/* IIGIPawn **/

	/** IIGISkeletalMesh */
	virtual USkeletalMeshComponent* GetMasterSkeletalMesh() override 
	{
		if (GISkeletalMesh)
			return GISkeletalMesh->GetMasterSkeletalMesh();
		return nullptr; 
	};
	virtual FVector GetSocketLocation(FName SocketNameIn) override 
	{
		if (GISkeletalMesh)
			return GISkeletalMesh->GetSocketLocation(SpawnSocketName);
		return FVector::ZeroVector; 
	};
	/* IIGISkeletalMesh **/
	/*
		Clieant pls, end cooldown right now!, I ended mine - Server.
	*/
	UFUNCTION(Client, Reliable)
		void EndCooldown();

	/*
		Use it to spawn cosmetics effects. Trails, impact effects, 
		ability irigin effects etc.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Cosmetic Events")
		void OnAbilityHit(const FVector& OriginOut, const FVector& HitLocationOut);

	virtual void EndCooldown_Implementation();
private:
	bool CheckStandalone();
};
