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

	ENetMode GetNetMode();
	/*
		Called when ability finished casting and will be fired.

		Also fired on each interval for channeled abilities ?
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnAbilityCasted();

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

	virtual UWorld* GetWorld() const override;

	virtual FVector GetCorrectedSocketLocation(FName SocketNameIn) { return FVector::ZeroVector; };

	UFUNCTION(BlueprintImplementableEvent, Category = "Cosmetic Events")
		void OnAbilityCastingStarted();

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
			return GISkeletalMesh->GetSocketLocation(SocketNameIn);
		return FVector::ZeroVector; 
	};
	/* IIGISkeletalMesh **/
	/*
		Clieant pls, end cooldown right now!, I ended mine - Server.
	*/
	UFUNCTION(Client, Reliable)
		void EndCooldown();

	UFUNCTION(BlueprintImplementableEvent, Category = "Cosmetic Events")
		void OnAbilityHit(const FVector& OriginOut, const FVector& HitLocationOut);

	virtual void EndCooldown_Implementation();
private:
	bool CheckStandalone();
};
