#pragma once
#include "GAGlobalTypes.h"
#include "Effects/GAGameEffect.h"
#include "GameplayTasksComponent.h"
#include "GameplayTask.h"
#include "GameplayTaskOwnerInterface.h"
#include "Attributes/GAAttributesBase.h"
#include "AFAbilityInterface.h"
#include "AFAbilityActivationSpec.h"
#include "AssetBundleData.h"
#include "SubclassOf.h"
#include "LatentActions/AFLatentInterface.h"

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
class ABILITYFRAMEWORK_API UGAAbilityBase : public UObject, public IAFAbilityInterface, public IAFLatentInterface
{
	GENERATED_BODY()
public:

	/*
		Since each ability is instanced per owner, and cannot be activated multiple times (ie, to run in background),
		we can assume that PredictionHandle will always be unique per activation.
	*/
	FAFPredictionHandle PredictionHandle;
	/* By default all abilities are considered to be replicated. */
	UPROPERTY(EditAnywhere, Category = "Replication")
		bool bReplicate;

	bool bIsNameStable;

	//possibly map TMap<FName, Task*> ?
	UPROPERTY(Transient)
		TSet<class UAFTaskBase*> ActiveTasks;
	/* List of tasks, this ability have. */
	UPROPERTY(Transient)
		TMap<FName, class UAFTaskBase*> AbilityTasks;

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

	UPROPERTY(Transient)
	class UWorld* World; 
	/*
		Replicated to everyone because we will need it, to determine cosmetic stuff on clients.
	*/
	/*
	*/
	UPROPERTY(BlueprintReadOnly, Replicated, BlueprintReadOnly, Category = "AbilityFramework|Abilities", Transient)
		APawn* POwner;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AbilityFramework|Abilities", Transient)
		APlayerController* PCOwner;
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AbilityFramework|Abilities", Transient)
		class AAIController* AICOwner;
	UPROPERTY(BlueprintReadOnly, Category = "AbilityFramework|Abilities", Transient)
		class UAFAbilityComponent* AbilityComponent;

	/* 
		Physical reprsentation of ability in game world. It might be sword, gun, or character.
		What differs it from pawn or controller is that Avatar is actually used by ability to perform actions.

		It will need some common interfaces for getting data out.
	*/
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "AbilityFramework|Abilities", Transient)
		class AActor* AvatarActor;
	
	UPROPERTY(BlueprintReadOnly, Category = "AbilityFramework|Abilities", Transient)
		UCameraComponent* OwnerCamera;

	FGAEffectContext DefaultContext;

	/*
		Tags applied to instigator of this ability, for duration of cooldown.
		Duration of this effect equals cooldown of ability.
	*/
	UPROPERTY(EditAnywhere, meta=(AllowedClass="AFAbilityCooldownSpec"), Category = "Config")
		FAFPropertytHandle CooldownEffect;
	TArray<FGAEffectHandle> CooldownEffectHandle;
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
	UPROPERTY(EditAnywhere, meta = (AllowedClass = "AFAbilityActivationSpec,AFAbilityPeriodSpec,AFAbilityInfiniteDurationSpec,AFAbilityPeriodicInfiniteSpec"), Category = "Config")
		FAFPropertytHandle ActivationEffect;
	TArray<FGAEffectHandle> ActivationEffectHandle;

	/*
		These attributes will be reduced by specified amount when ability is activated.
		Attribute cost from Ability Owner attributes
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		TArray<FAFPropertytHandle> AttributeCost;
	TArray<TArray<FGAEffectHandle>> AttributeCostHandle;
	/*
		Attribute cost from ability own attributes
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		TArray<FAFPropertytHandle> AbilityAttributeCost;
	TArray<TArray<FGAEffectHandle>> AbilityAttributeCostHandle;

	UPROPERTY(AssetRegistrySearchable)
		FName AbilityTagSearch;

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

public: //because I'm to lazy to write all those friend states..
	UFUNCTION()
		void OnActivationEffectPeriod();

	/* Replication counters for above events. */


	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnInputPressedDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnInputReleasedDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnActivateBeginDelegate;
	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnActivationFinishedDelegate;

	UPROPERTY(BlueprintAssignable)
		FGASGenericAbilityDelegate OnNotifyOnCooldown;

	/* Stub, I think replicating montage directly from ability will be better, as abilities are replicated regardless. */
	UPROPERTY(Transient)
		UAnimMontage* RepMontage;
protected:
	EAFAbilityState AbilityState;

public:
	UGAAbilityBase(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitProperties() override;
	
	virtual void Serialize(FArchive& Ar) override;
	// UObject interface
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	virtual void PostLoad() override;

#if WITH_EDITORONLY_DATA
	/** This scans the class for AssetBundles metadata on asset properties and initializes the AssetBundleData with InitializeAssetBundlesFromMetadata */
	virtual void UpdateAssetBundleData();

	/** Updates AssetBundleData */
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;

protected:
	/** Asset Bundle data computed at save time. In cooked builds this is accessible from AssetRegistry */
	UPROPERTY()
		FAssetBundleData AssetBundleData;
#endif
public:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	void UpdateAssetRegistryInfo();

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed = 1);

	void InitAbility();
public:
	UFUNCTION()
		void OnAttributeSetReplicated(class UGAAttributesBase* ReplicatedAttributes);
	//called on both server and client after InitAbility();
	virtual void OnAbilityInited();

	/*
		Called on server and client, after ability has been bound to some input.
	*/
	virtual void OnAbilityInputReady() {};

	/*
	 * @call Order:
	 * Previous Function: FGASAbilityContainer::HandleInputPressed
	 * Next Function: UGAAbilityBase::OnInputPressed
	 *
	 * Called on both Client and Server.
	 *
	 * @param ActionName - Name of action which tirggered this ability
	 * @param InPredictionHandle - Prediction Handle Generate By Client
	 */
	void OnNativeInputPressed(uint8 InputID, const FAFPredictionHandle& InPredictionHandle);
	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::OnNativeInputPressed
	* Next Function: Multiple Choices. Next function is usually called from within Ability Blueprint
	* Default Choices:
	* UGAAbilityBase::StartActivation
	* UGAAbilityBase::CanUseAbility
	* Custom Function
	* 
	* Called on both Client and Server.
	*
	* @param ActionName - Name of action which tirggered this ability
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnInputPressed(uint8 InputID);

	void OnNativeInputReleased(uint8 InputID);
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnInputReleased(uint8 InputID);


	

	void NativeOnAbilityConfirmed();



	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::StartActivation
	* Next Function: (Blueprint) UGAAbilityBase::FinishAbility
	*
	* Called on both Client and Server to indicate that ability is finished.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivationFinished();

	/*
	* @call Order:
	* Previous Function: (Blueprint) UGAAbilityBase::OnInputPressed
	* Next Function: UGAAbilityBase::NativeOnBeginAbilityActivation
	*
	* Called on both Client and Server.
	*
	* @param bApplyActivationEffect - Should apply activation effect to Owner.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void StartActivation(bool bApplyActivationEffect);

	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::StartActivation
	* Next Function: UGAAbilityBase::ApplyActivationEffect
	* Next Function: (Blueprint) UGAAbilityBase::OnActivate
	* 
	* Called on both Client and Server.
	*
	* @param bApplyActivationEffect - Should apply activation effect to Owner.
	*/
	virtual void NativeOnBeginAbilityActivation(bool bApplyActivationEffect);

	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::NativeOnBeginAbilityActivation -
	* called when activation effect finishes (or immedietly, if there was no activation effect applied).
	* Next Function: (Blueprint) Custom Functions
	* Next Function: (Blueprint) UGAAbilityBase::FinishAbility - must be called at some point
	* finish ability.
	*
	* Called on both Client and Server.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivate();

	/* Event called when ability activation has been canceled. */
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnActivationCancel();
	/*
	* @call Order:
	* Previous Function: Called if Periodic effect has been applied and is active. Otherwise inactive.
	* called when activation effect finishes (or immedietly, if there was no activation effect applied).
	* Next Function: (Blueprint) Custom Functions
	*
	* Called on both Client and Server.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnPeriod();

	

	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnCooldownStart();
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnCooldownEnd();

	void NativeOnCooldownEnd();

	UFUNCTION()
		void OnCooldownEffectExpired();
	UFUNCTION()
		void NativeOnAbilityActivationFinish();
	UFUNCTION()
		void NativeOnAbilityActivationCancel();

	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::NativeOnBeginAbilityActivation -
	* called when activation effect finishes (or immedietly, if there was no activation effect applied).
	* Next Function: (Blueprint) Custom Functions
	* Next Function: (Blueprint) UGAAbilityBase::FinishAbility - must be called at some point
	* finish ability.
	*
	* Called to finish ability and start clean up.
	*/
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void FinishAbility();
	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::FinishAbility
	* Next Function: (Blueprint) UGAAbilityBase::OnAbilityFinished
	* 
	* Called to finish ability and start clean up.
	*/
	void NativeFinishAbility();

	/*
	* @call Order:
	* Previous Function: UGAAbilityBase::NativeFinishAbility
	* Next Function: (Blueprint) Custom Functions
	*
	* Called when ability is finished.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "AbilityFramework|Abilities")
		void OnAbilityFinished();
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

	UFUNCTION(BlueprintPure, meta = (DisplayName = "Can Use Ability"), Category = "AbilityFramework|Abilities")
		bool BP_CanUseAbility();
	
	virtual void SetAttributes(UGAAttributesBase* InAttributes)
	{
		Attributes = InAttributes;
	}
	virtual class UGAAttributesBase* GetAttributes() const
	{
		return Attributes;
	}
	/** IAFAbilityInterface Begin */
	virtual class UGAAttributesBase* GetAttributes() override;
	virtual class UAFAbilityComponent* GetAbilityComp() override;
	virtual class UAFEffectsComponent* GetEffectsComponent() override;
	virtual class UAFEffectsComponent* NativeGetEffectsComponent() const override;
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Attributes")
	virtual float GetAttributeValue(FGAAttribute AttributeIn) const override;
	virtual float NativeGetAttributeValue(const FGAAttribute AttributeIn) const override;
	virtual FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) override { return Attributes->GetAttribute(AttributeIn); };
	virtual void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) override { Attributes->RemoveBonus(AttributeIn, HandleIn, InMod); };
	virtual void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	, FGAEffectProperty& InProperty
	, const FGAEffectContext& InContext) override
	{ 
		if (!Attributes)
		{
			UE_LOG(AFAbilities, Log, TEXT("ModifyAttribute Ability Attributes INVALID"));
			return;
		}
		Attributes->ModifyAttribute(ModIn, HandleIn, InProperty, InContext);
	};
	virtual FAFPredictionHandle GetPredictionHandle() override;
	/* IAFAbilityInterface End **/
	UFUNCTION(BlueprintPure, Category = "AbilityFramework|Abilities|Attributes")
		virtual float GetAttributeVal(FGAAttribute AttributeIn) const;

public: //protected ?
	bool ApplyCooldownEffect();
	UFUNCTION(Client, Reliable)
		void ClientSetCooldownHandle(FGAEffectHandle InCooldownHandle);
	void ClientSetCooldownHandle_Implementation(FGAEffectHandle InCooldownHandle);

	bool ApplyActivationEffect(bool bApplyActivationEffect);
	bool ApplyAttributeCost();
	bool ApplyAbilityAttributeCost();
	bool CheckAbilityAttributeCost();
	bool CheckAttributeCost();
	bool IsOnCooldown();
	bool IsActivating();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Is On Cooldown"), Category = "AbilityFramework|Abilities")
		bool BP_IsOnCooldown();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Cooldown"), Category = "AbilityFramework|Abilities")
		void BP_ApplyCooldown();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_ApplyAttributeCost();
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_CheckAttributeCost();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Apply Ability Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_ApplyAbilityAttributeCost();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Check Ability Attribute Cost"), Category = "AbilityFramework|Abilities")
		bool BP_CheckAbilityAttributeCost();

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

public:
	int32 GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void ExecuteAbilityInputPressedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName);
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities")
		void ExecuteAbilityInputReleasedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName);

	virtual class UWorld* GetWorld() const override;

	inline void AddAbilityTask(FName InName, class UAFTaskBase* InTask)
	{
		if (!AbilityTasks.Contains(InName))
		{
			AbilityTasks.Add(InName, InTask);
		}
	}
	class UGAAbilityTask* GetAbilityTask(const FName& InName);

	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tags")
		bool HaveGameplayTag(AActor* Target, const FGameplayTag& Tag);
	UFUNCTION(BlueprintCallable, Category = "AbilityFramework|Abilities|Tags")
		bool HaveAnyGameplayTag(AActor* Target, const FGameplayTagContainer& Tag);

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


	//Helpers
	float GetActivationRemainingTime() const;
	float GetActivationRemainingTimeNormalized() const;
	float GetActivationCurrentTime() const;
	float GetActivationCurrentTimeNormalized() const;
	float GetActivationEndTime() const;

	UFUNCTION(BlueprintPure, DisplayName = "GetActivationRemainingTime", Category = "AbilityFramework|Abilities|Helpers")
		float BP_GetActivationRemainingTime();
	UFUNCTION(BlueprintPure, DisplayName = "GetActivationRemainingTimeNormalized", Category = "AbilityFramework|Abilities|Helpers")
		float BP_GetActivationRemainingTimeNormalized();
	UFUNCTION(BlueprintPure, DisplayName = "GetActivationCurrentTime", Category = "AbilityFramework|Abilities|Helpers")
		float BP_GetActivationCurrentTime();
	UFUNCTION(BlueprintPure, DisplayName = "GetActivationCurrentTimeNormalized", Category = "AbilityFramework|Abilities|Helpers")
		float BP_GetActivationCurrentTimeNormalized();
	UFUNCTION(BlueprintPure, DisplayName = "GetActivationEndTime", Category = "AbilityFramework|Abilities|Helpers")
		float BP_GetActivationEndTime();


	float GetCooldownRemainingTime() const;
	float GetCooldownRemainingTimeNormalized() const;
	float GetCooldownCurrentTime() const;
	float GetCooldownCurrentTimeNormalized() const;
	float GetCooldownEndTime() const;

	UFUNCTION(BlueprintPure, DisplayName = "GetCooldownRemainingTime", Category = "AbilityFramework|Abilities|Helpers")
		float  BP_GetCooldownRemainingTime();
	UFUNCTION(BlueprintPure, DisplayName = "GetCooldownRemainingTimeNormalized", Category = "AbilityFramework|Abilities|Helpers")
		float  BP_GetCooldownRemainingTimeNormalized();
	UFUNCTION(BlueprintPure, DisplayName = "GetCooldownCurrentTime", Category = "AbilityFramework|Abilities|Helpers")
		float  BP_GetCooldownCurrentTime();
	UFUNCTION(BlueprintPure, DisplayName = "GetCooldownCurrentTimeNormalized", Category = "AbilityFramework|Abilities|Helpers")
		float  BP_GetCooldownCurrentTimeNormalized();
	UFUNCTION(BlueprintPure, DisplayName = "GetCooldownEndTime", Category = "AbilityFramework|Abilities|Helpers")
		float  BP_GetCooldownEndTime();

	UFUNCTION(BlueprintCallable, DisplayName = "Get Avatar", Category = "AbilityFramework|Abilities|Helpers")
		AActor*  BP_GetAvatar();

	virtual void OnAvatarReady() {};


	/*  IAFLatentInterface */
	virtual void OnLatentTaskAdded(FName InstanceName, class UAFTaskBase* TaskIn);
	virtual void AddReplicatedTask(class UAFTaskBase* TaskIn);
	virtual void OnLatentTaskRemoved(class UAFTaskBase* TaskIn);

	virtual void OnLatentTaskActivated(class UAFTaskBase* TaskIn);
	virtual void OnLatentTaskDeactivated(class UAFTaskBase* TaskIn);

	virtual class UAFTaskBase* GetCachedLatentAction(FName TaskName);
	/*  IAFLatentInterface */
};
