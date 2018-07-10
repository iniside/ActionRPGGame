// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "GameplayTagAssetInterface.h"

#include "Attributes/GAAttributeBase.h"
#include "Attributes/GAAttributesBase.h"
#include "Effects/GAEffectCueGlobals.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"

#include "AFEffectsComponent.generated.h"

DECLARE_DELEGATE(FAFEffectEvent);
DECLARE_DELEGATE_OneParam(FAFEventDelegate, FAFEventData);

DECLARE_MULTICAST_DELEGATE_ThreeParams(FAFApplicationDelegate, FGAEffectContext, FAFPropertytHandle, FAFEffectSpec);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ABILITYFRAMEWORK_API UAFEffectsComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	friend class UGAAbilityBase;
	friend class IAFAbilityInterface;
	friend class UAFAbilityComponent;
	friend class UAFEffectTask_EffectEvent;

private:
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DefaultTags;

	UPROPERTY(Replicated)
		FGACountedTagContainer AppliedTags;

	UPROPERTY(Replicated)
		FGACountedTagContainer ImmunityTags;

	UPROPERTY(ReplicatedUsing = OnRep_GameEffectContainer)
		FGAEffectContainer GameEffectContainer;

	TMap<FGameplayTag, FAFEffectEvent> OnEffectApplyToTarget;
	TMap<FGameplayTag, FAFEffectEvent> OnEffectApplyToSelf;
	TMap<FGameplayTag, TArray<FAFEventDelegate>> EffectEvents;

	TMap<FGAEffectHandle, FAFCueHandle> EffectToCue;
	
	TMap<FGameplayTag, FSimpleDelegate> OnEffectEvent;

	TMap<FGameplayTag, TArray<FAFEventDelegate>> AppliedEvents;
	TMap<FGameplayTag, TArray<FAFEventDelegate>> ExecutedEvents;
public:
	FAFApplicationDelegate OnAppliedToTarget;
	FAFApplicationDelegate OnAppliedToSelf;
	FAFApplicationDelegate OnEffectExecuted;
public:	
	// Sets default values for this component's properties
	UAFEffectsComponent(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	/** UActorComponent Interface - Begin */
	virtual void InitializeComponent() override;
	/* UActorComponent Interface - End **/

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:

	/*
	* @call Order:
	* Previous Function: UAFAbilityComponent::ApplyEffectToTarget
	* Next Function: FGAEffectContainer::ApplyEffect
	* Apply target to Me. Try to apply effect to container and launch Events in:
	* TMap<FGameplayTag, FSimpleDelegate> OnEffectEvent - event is called before application;
	* TMap<FGameplayTag, FAFEffectEvent> OnEffectApplyToSelf - event is called before application;
	*
	* @param EffectIn& - Effect to apply
	* @param InProperty - cached effect information
	* @param InContext - Context about effect application. Target, instigator, causer.
	* @param Modifier - optional modifier which can be applied to effect.
	* @return Handle to Effect;
	*/
	FGAEffectHandle ApplyEffectToSelf(
		const FGAEffect& EffectIn
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

	/*
	* @call Order:
	* Previous Function: UGABlueprintLibrary::ApplyEffect
	* Next Function: UAFAbilityComponent::ApplyEffectToSelf
	* Apply effect to target provided inside Context.
	* Try launch events:
	* TMap<FGameplayTag, FAFEffectEvent> OnEffectApplyToTarget - event is called before application
	*
	* @param EffectIn& - Effect to apply
	* @param InProperty - cached effect information
	* @param InContext - Context about effect application. Target, instigator, causer.
	* @param Modifier - optional modifier which can be applied to effect.
	* @return Handle to Effect;
	*/
	FGAEffectHandle ApplyEffectToTarget(
		const FGAEffect& EffectIn
		, const FAFEffectParams& Params
		, const FAFFunctionModifier& Modifier = FAFFunctionModifier());

public:
	/* Have to to copy handle around, because timer delegates do not support references. */
	void ExecuteEffect(FGAEffectHandle HandleIn
		, FAFEffectParams Params
		, FAFFunctionModifier Modifier);
	
	virtual void PostExecuteEffect();
	/* ExpireEffect is used to remove existing effect naturally when their time expires. */
public:
	void ExpireEffect(FGAEffectHandle HandleIn
		, FAFEffectParams Params);

protected:
	UFUNCTION(Client, Reliable)
		void ClientExpireEffect(FAFPredictionHandle PredictionHandle);
	void ClientExpireEffect_Implementation(FAFPredictionHandle PredictionHandle);

	/* RemoveEffect is used to remove effect by force. */
	void RemoveEffect(const FAFPropertytHandle& InProperty
		, const FGAEffectContext& InContext
		, const FGAEffectHandle& InHandle);
	void InternalRemoveEffect(const FAFPropertytHandle& InProperty, const FGAEffectContext& InContext);
	
	void AddEffectEvent(const FGameplayTag& InEventTag, const FSimpleDelegate& InEvent);
	void ExecuteEffectEvent(const FGameplayTag& InEventTag);
	void RemoveEffectEvent(const FGameplayTag& InEventTag);
	bool IsEffectActive(const FGAEffectHandle& InHandle) const;

public:
	void AddEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate);
	void RemoveEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate);
	TArray<FAFEventDelegate>& GetTagEvent(FGameplayTag TagIn);
	void NativeTriggerTagEvent(FGameplayTag TagIn, const FAFEventData& InEventData);

	void AddAppliedEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate);
	void RemoveAppliedEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate);
	void TriggerAppliedEvent(FGameplayTag TagIn, const FAFEventData& InEventData);


	void AddExecuteEvent(const FGameplayTag& EventTag, FAFEventDelegate& EventDelegate);
	void RemoveExecuteEvent(const FGameplayTag& EventTag, const FDelegateHandle& EventDelegate);
	void TriggerExecuteEvent(FGameplayTag TagIn, const FAFEventData& InEventData);

public:
	bool DenyEffectApplication(const FGameplayTagContainer& InTags);
	bool HaveEffectRquiredTags(const FGameplayTagContainer& InTags);
protected:
	/*

	*/
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastApplyEffectCue(FGAEffectCueParams CueParams, FAFCueHandle InHandle);
	virtual void MulticastApplyEffectCue_Implementation(FGAEffectCueParams CueParams, FAFCueHandle InHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExecuteEffectCue(TSubclassOf<UGAGameEffectSpec> EffectSpec, FGAEffectCueParams CueParams);
	void MulticastExecuteEffectCue_Implementation(TSubclassOf<UGAGameEffectSpec> EffectSpec, FGAEffectCueParams CueParams);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRemoveEffectCue(FGAEffectCueParams CueParams, FAFCueHandle InHandle);
	void MulticastRemoveEffectCue_Implementation(FGAEffectCueParams CueParams, FAFCueHandle InHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateDurationCue(FGAEffectHandle EffectHandle, float NewDurationIn);
	void MulticastUpdateDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdatePeriodCue(FGAEffectHandle EffectHandle, float NewPeriodIn);
	void MulticastUpdatePeriodCue_Implementation(FGAEffectHandle EffectHandle, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateTimersCue(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn);
	void MulticastUpdateTimersCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExtendDurationCue(FGAEffectHandle EffectHandle, float NewDurationIn);
	void MulticastExtendDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn);
public:
	FGAEffect* GetEffect(const FGAEffectHandle& InHandle);

	/* Counted Tag Container Wrapper Start */
	inline void AddTag(const FGameplayTag& TagIn) { AppliedTags.AddTag(TagIn); };
	inline void AddTagContainer(const FGameplayTagContainer& TagsIn) { AppliedTags.AddTagContainer(TagsIn); };
	inline void RemoveTag(const FGameplayTag& TagIn) { AppliedTags.RemoveTag(TagIn); };
	inline void RemoveTagContainer(const FGameplayTagContainer& TagsIn) { AppliedTags.RemoveTagContainer(TagsIn); };
	inline bool HasTag(const FGameplayTag& TagIn) const { return AppliedTags.HasTag(TagIn); }
	inline bool HasTagExact(const FGameplayTag TagIn) const { return AppliedTags.HasTagExact(TagIn); };
	inline bool HasAny(const FGameplayTagContainer& TagsIn) const { return AppliedTags.HasAny(TagsIn); };
	inline bool HasAnyExact(const FGameplayTagContainer& TagsIn) const { return AppliedTags.HasAnyExact(TagsIn); };
	inline bool HasAll(const FGameplayTagContainer& TagsIn) const { return AppliedTags.HasAll(TagsIn); };
	inline bool HasAllExact(const FGameplayTagContainer& TagsIn) const { return AppliedTags.HasAllExact(TagsIn); };
	inline int32 GetTagCount(const FGameplayTag& TagIn) const { return AppliedTags.GetTagCount(TagIn); }
	/* Counted Tag Container Wrapper Start */

	
	/* Game Effect Container WRAPPER */
	bool IsEffectActive(TSubclassOf<UGAGameEffectSpec> EffectClass)
	{
		return GameEffectContainer.IsEffectActive(EffectClass);
	}

	/* Game Effect Container WRAPPER */

	/*Network Functions - BEGIN */
protected:
	UFUNCTION()
		void OnRep_GameEffectContainer();

	/*Network Functions - END */
};
