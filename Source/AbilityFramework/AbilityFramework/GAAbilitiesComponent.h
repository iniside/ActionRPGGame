// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameplayTaskOwnerInterface.h"
#include "GameplayTaskTypes.h"
#include "GameplayTask.h"
#include "GameplayTasksComponent.h"
#include "GameplayTags.h"
#include "GAGlobals.h"
#include "Attributes/GAAttributeBase.h"
#include "Attributes/GAAttributesBase.h"
#include "Effects/GAEffectCueGlobals.h"

#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "Messaging.h"
#include "GameplayTagAssetInterface.h"
#include "IGAAbilities.h"

//tests
#include "Async/AsyncUObjectImpl.h"
#include "Async/AsyncUObject.h"
#include "HAL/RunnableThread.h"

#include "GAAbilitiesComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("AttributeComponent"), STATGROUP_AttributeComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentApplyEffect"), STAT_ApplyEffect, STATGROUP_AttributeComponent, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentModifyAttribute"), STAT_ModifyAttribute, STATGROUP_AttributeComponent, );

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAOnAttributeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeModifed, const FGAModifiedAttribute&, attr);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGAGenericEffectDelegate, const FGAEffectHandle&, Handle, const FGameplayTagContainer&, Tags);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeChanged2, float, NewValue);

USTRUCT()
struct FGAModifiedAttributeData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGAModifiedAttribute> Mods;
	UPROPERTY()
		int32 ForceUpdate;

	FGAModifiedAttributeData()
		: ForceUpdate(0)
	{}
};

USTRUCT(BlueprintType)
struct FGAEffectUIData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "UI")
		float RemainingTime;

	FGAEffectUIData()
		: RemainingTime(0)
	{};
};
struct FGAContextSetup
{
public:
	UGAAttributesBase* IntigatorAttributes;
	UGAAttributesBase* TargetAttributes;
	UGAAbilitiesComponent* InstigatorComp;
	UGAAbilitiesComponent* TargetComp;

	FGAContextSetup()
	{};
	FGAContextSetup(UGAAttributesBase* IntigatorAttributesIn, UGAAttributesBase* TargetAttributesIn,
		UGAAbilitiesComponent* InstigatorCompIn, UGAAbilitiesComponent* TargetCompIn)
		: IntigatorAttributes(IntigatorAttributesIn),
		TargetAttributes(TargetAttributesIn),
		InstigatorComp(InstigatorCompIn),
		TargetComp(TargetCompIn)
	{};
};
DECLARE_MULTICAST_DELEGATE_TwoParams(FGASOnActiveAbilityAdded, int32, int32);
DECLARE_DELEGATE_OneParam(FGASMontageGenericDelegate, const FGASAbilityNotifyData&);
/* TODO:: Implement fast serialization for structs. */
/* TODO:: REmove all those structs for customization and replace it with something sane like tmap. */
/**/

USTRUCT(BlueprintType)
struct ABILITYFRAMEWORK_API FGASMontageRepData
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		UAnimMontage* CurrentMontage;

	UPROPERTY()
		FName SectionName;

	UPROPERTY()
		uint8 ForceRep;
};

USTRUCT()
struct ABILITYFRAMEWORK_API FGASAbilityItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UPROPERTY()
		UGAAbilityBase* Ability;

	void PreReplicatedRemove(const struct FGASAbilityContainer& InArraySerializer);
	void PostReplicatedAdd(const struct FGASAbilityContainer& InArraySerializer);
	void PostReplicatedChange(const struct FGASAbilityContainer& InArraySerializer);
};
USTRUCT()
struct ABILITYFRAMEWORK_API FGASAbilityContainer : public FFastArraySerializer
{
	GENERATED_BODY()
public:

	UPROPERTY()
		TArray<FGASAbilityItem> AbilitiesItems;

	TWeakObjectPtr<class UGAAbilitiesComponent> AbilitiesComp;

	TMap<FGameplayTag, UGAAbilityBase*> AbilitiesInputs;

	UGAAbilityBase* AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn, FGameplayTag ActionName);
	UGAAbilityBase* GetAbility(FGameplayTag TagIn);
	
	void HandleInputPressed(FGameplayTag TagIn, FGameplayTag ActionName);
	void HandleInputReleased(FGameplayTag TagIn, FGameplayTag ActionName);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FGASAbilityItem, FGASAbilityContainer>(AbilitiesItems, DeltaParms, *this);
	}
};

template<>
struct TStructOpsTypeTraits< FGASAbilityContainer > : public TStructOpsTypeTraitsBase2<FGASAbilityContainer>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class ABILITYFRAMEWORK_API UGAAbilitiesComponent : public UGameplayTasksComponent, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
		/* Attributes handling */
public:
	friend struct FAFMessageTick;
	FAsyncUObjectRunnable* TestRunnable;
	FRunnableThread* RouterThread;
	UAsyncUObject* TEstAsyncUObject;
	UPROPERTY(EditAnywhere, Category = "Test")
		FGameplayTag TagTest;
	/*
	Set attribute which will be considered for indicating whether or not actor is dead.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAAttribute DeathAttribute;

	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer DefaultTags;

	UPROPERTY()
		FGACountedTagContainer AppliedTags;

	UFUNCTION()
		void OnRep_ActiveEffects();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveCues)
		FGameCueContainer ActiveCues;
	UFUNCTION()
		void OnRep_ActiveCues();
	/*
	Could make it array. But realistically. How many times do you really need more, than one
	attribute set for actor ?

	Of course array of attributes would allow to compose attributes from small discreete
	attribute sets. On the other hand similiar funcionality can be achieved by using
	inheritance.

	And I think that using inheritance in this case will be easier.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Replicated)
		class UGAAttributesBase* DefaultAttributes;

	TMap<FGameplayTag, class UGAAttributesBase*> AdditionalAttributes;

	UPROPERTY(ReplicatedUsing = OnRep_AttributeChanged)
		FGAModifiedAttributeData ModifiedAttribute;
	UFUNCTION()
		void OnRep_AttributeChanged();
	UPROPERTY(BlueprintAssignable, Category = "Game Attributes")
		FGAOnAttributeModifed OnAttributeModifed;

	UPROPERTY(BlueprintAssignable, Category = "Game Attributes")
		FGAOnAttributeChanged2 OnAttributeChanged;
	/* Effect/Attribute System Delegates */
	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectApplied;

	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectApplyToTarget;

	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectApplyToSelf;

	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectExecuted;
	/* Called when efect period ticked. */
	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectTicked;

	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectExpired;

	UPROPERTY(BlueprintAssignable, Category = "Effect")
		FGAGenericEffectDelegate OnEffectRemoved;

	/* NEW EFFECT SYSTEM */
	UPROPERTY(ReplicatedUsing = OnRep_GameEffectContainer)
		FGAEffectContainer GameEffectContainer;

	/*
		Default effects applied when character spawns.
		Can contain things like attribute regen, racial bonuses
		racial debuffs etc.
	*/
	UPROPERTY(EditAnywhere, Category = "Effects")
		TArray<TSubclassOf<UGAGameEffectSpec>> DefaultEffects;

	TMap<FGameplayTag, FGAGenericDelegate> GenericTagEvents;

	virtual bool GetShouldTick() const override;

	UFUNCTION()
		void OnRep_GameEffectContainer();

	template<typename T>
	T* GetAttributes()
	{
		return CastChecked<T>(DefaultAttributes);
	}

	template<class T>
	T* GetAttributeSet(FGameplayTag InOwner)
	{
		UGAAttributesBase* AttributeSet = AdditionalAttributes.FindRef(InOwner);
		return Cast<T>(AttributeSet);
	}
	void AddAddtionalAttributes(FGameplayTag InOwner, UGAAttributesBase* InAttributes)
	{
		bool bExists = AdditionalAttributes.Contains(InOwner);
		if (bExists)
		{
			return;
		}
		AdditionalAttributes.Add(InOwner, InAttributes);
	}
	UFUNCTION(BlueprintCallable, Category = "Test")
		void GetAttributeStructTest(FGAAttribute Name);

	virtual void BeginPlay() override;
	virtual void DestroyComponent(bool bPromoteChildren = false) override;

	virtual void InitializeComponent() override;

	virtual void UninitializeComponent() override;


public:
	/////////////////////////////////////////////////
	//////////// EFFECTS HANDLING
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void Update();
	FGAEffectHandle ApplyEffectToSelf(const FGAEffect& EffectIn,
		const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);
	FGAEffectHandle ApplyEffectToTarget(const FGAEffect& EffectIn,
		const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);

	void ApplyEffectToTarget(TSubclassOf<UGAGameEffectSpec> InSpecClass, 
		const FGAEffectContext& InContext, const FGAEffectHandle& InHandle);

	FGAEffectHandle MakeGameEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn,
		const FGAEffectContext& ContextIn);

	/* Have to to copy handle around, because timer delegates do not support references. */
	void ExecuteEffect(FGAEffectHandle HandleIn, FGAEffectProperty InProperty);
	/* ExpireEffect is used to remove existing effect naturally when their time expires. */
	void ExpireEffect(FGAEffectHandle HandleIn);
	/* RemoveEffect is used to remove effect by force. */
	void RemoveEffect(const FGAEffectHandle& HandleIn);
	void InternalRemoveEffect(const FGAEffectHandle& HandleIn);

	void ApplyInstacnedEffectToSelf(class UGAEffectExtension* EffectIn);
	void ApplyInstancedToTarget(class UGAEffectExtension* EffectIn);

	void RemoveInstancedFromSelf(class UGAEffectExtension* EffectIn);
	/* Never call it directly. */
	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		TArray<FGAEffectUIData> GetEffectUIData();

	/*
	Get Last Index of effect for UI display.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		int32 GetEffectUIIndex();

	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		FGAEffectUIData GetEffectUIDataByIndex(int32 IndexIn);
	/*
	Need prediction for spawning effects on client,
	and then on updateing them predicitvely on all other clients.
	*/
	/*

	*/
	UFUNCTION(NetMulticast, Unreliable)
		void MulticastApplyEffectCue(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExecuteEffectCue(FGAEffectHandle EffectHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRemoveEffectCue(FGAEffectHandle EffectHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateDurationCue(FGAEffectHandle EffectHandle, float NewDurationIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdatePeriodCue(FGAEffectHandle EffectHandle, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateTimersCue(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExtendDurationCue(FGAEffectHandle EffectHandle, float NewDurationIn);

	//////////// EFFECTS HANDLING
	/////////////////////////////////////////////////

public:
	/////////////////////////////////////////////////
	//////////// ATTRIBUTES HANDLING

	/*
	Two functions, They will allow to apply any static numerical mods from player who
	initiated attribute change, and from player who will be affected by change.

	Mods will be appiled by small objects, and changed against tags.
	For example there might be physical armor mod, which will apply changes only
	to attributes tagged as Damage.Physical and only if you are reciving change, not causing it.
	*/

	inline float GetFinalAttributeValue(const FGAAttribute& Name)
	{
		return DefaultAttributes->GetFinalAttributeValue(Name);
	}
	inline float GetCurrentAttributeValue(const FGAAttribute& Name)
	{
		return DefaultAttributes->GetCurrentAttributeValue(Name);
	}
	//////////// ATTRIBUTES HANDLING
	/////////////////////////////////////////////////
	/*
	Attribute replication.
	*/
	void OnAttributeModified(const FGAEffectMod& InMod, const FGAEffectHandle& InHandle, UGAAttributesBase* InAttributeSet);

	FGAGenericDelegate& GetTagEvent(FGameplayTag TagIn);
	void NativeTriggerTagEvent(FGameplayTag TagIn);
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		void BP_TriggerTagEvent(FGameplayTag TagIn);
	//Helper functions:
public:
	/*
	IGameplayTagAssetInterface Start
	*/
	/**
	* Get any owned gameplay tags on the asset
	*
	* @param OutTags	[OUT] Set of tags on the asset
	*/
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	/**
	* Check if the asset has a gameplay tag that matches against the specified tag (expands to include parents of asset tags)
	*
	* @param TagToCheck	Tag to check for a match
	*
	* @return True if the asset has a gameplay tag that matches, false if not
	*/
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	/**
	* Check if the asset has gameplay tags that matches against all of the specified tags (expands to include parents of asset tags)
	*
	* @param TagContainer			Tag container to check for a match
	*
	* @return True if the asset has matches all of the gameplay tags, will be true if container is empty
	*/
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

	/**
	* Check if the asset has gameplay tags that matches against any of the specified tags (expands to include parents of asset tags)
	*
	* @param TagContainer			Tag container to check for a match
	*
	* @return True if the asset has matches any of the gameplay tags, will be false if container is empty
	*/
	UFUNCTION(BlueprintCallable, Category = GameplayTags)
		virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	/*
	IGameplayTagAssetInterface End
	*/
	/*
	Effect Container Wrapp Start
	*/
	/*
	*/
	inline bool IsEffectActive(const FGAEffectHandle& HandleIn) { return GameEffectContainer.IsEffectActive(HandleIn); };
	/*
	Effect Container Wrapp End
	*/

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

	/* Active Effects Wrapper Start */
	inline int32 GetEffectsNum() const { return GameEffectContainer.GetEffectsNum(); };
	/* Active Effects Wrapper End */

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		class UGAAttributesBase* GetAttributes() { return DefaultAttributes; };

	UFUNCTION(BlueprintCallable, Category = "Game Attributes")
		float GetAttributeValue(FGAAttribute AttributeIn) const { return DefaultAttributes->GetCurrentAttributeValue(AttributeIn); };

	void ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn, FGAEffectProperty& InProperty);// { DefaultAttributes->ModifyAttribute(ModIn, HandleIn); };
	
	FAFAttributeBase* GetAttribute(FGAAttribute AttributeIn) { return DefaultAttributes->GetAttribute(AttributeIn); };
	void RemoveBonus(FGAAttribute AttributeIn, const FGAEffectHandle& HandleIn, EGAAttributeMod InMod) { DefaultAttributes->RemoveBonus(AttributeIn, HandleIn, HandleIn.GetAttributeMod());  };
	float NativeGetAttributeValue(const FGAAttribute AttributeIn) const { return 0; };

private:
	class IIGAAbilities* AttributeInterface;

public:
	UGAAbilitiesComponent(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
		void OnRep_InstancedAbilities();
	UPROPERTY(Replicated)
		FGASAbilityContainer AbilityContainer;
	UPROPERTY()
		TArray<UGAAbilityBase*> AbilitiesRefs;

	UPROPERTY()
		APlayerController* PCOwner;

	/* Ability which is currently being executed. */
	UPROPERTY(BlueprintReadOnly, Category = "Game Abilities")
		class UGAAbilityBase* ExecutingAbility;

	TMap<FGameplayTag, FMessageAddress> AbilitiesToConfirm;

	/*
		True if player is currently casting/channeling/activating(?)
		any ability.
	*/
	bool bIsAnyAbilityActive;

	FGASMontageGenericDelegate OnAbilityStartNotify;
	FGASMontageGenericDelegate OnAbilityEndNotify;
	FGASMontageGenericDelegate OnAbilityNotify;
	FGASMontageGenericDelegate OnAbilityNotifyStateStart;
	FGASMontageGenericDelegate OnAbilityNotifyStateTick;
	FGASMontageGenericDelegate OnAbilityNotifyStateEnd;
	
	class IIGIPawn* PawnInterface;
private:
	

	UPROPERTY(ReplicatedUsing=OnRep_PlayMontage)
		FGASMontageRepData RepMontage;
	UFUNCTION()
		void OnRep_PlayMontage();
public:
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
	void PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed = 1);
public:
	/*
		Checks on client and server, if we can activate ability. Called from InputPressed
	*/
	virtual bool CanActivateAbility();
	
	inline class UGAAbilityBase* GetGASAbility(int32 IndexIn)
	{
		return nullptr;
	}
	UFUNCTION(BlueprintCallable, Category = "Game Abilities")
		void BP_BindAbilityToAction(FGameplayTag ActionName, FGameplayTag AbilityTag);
	void BindAbilityToAction(UInputComponent* InputComponent, FGameplayTag ActionName, FGameplayTag AbilityTag);
	
	UFUNCTION(BlueprintCallable, meta=(DisplayName="Input Pressed"), Category = "Game Abilities")
		void BP_InputPressed(FGameplayTag AbilityTag, FGameplayTag ActionName);

	void NativeInputPressed(FGameplayTag AbilityTag, FGameplayTag ActionName);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNativeInputPressed(FGameplayTag AbilityTag, FGameplayTag ActionName);
	virtual void ServerNativeInputPressed_Implementation(FGameplayTag AbilityTag, FGameplayTag ActionName);
	virtual bool ServerNativeInputPressed_Validate(FGameplayTag AbilityTag, FGameplayTag ActionName);
	void SendAbilityActivationMessage(FGameplayTag AbilityTag, FGameplayTag ActionName);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Input Released"), Category = "Game Abilities")
		void BP_InputReleased(FGameplayTag AbilityTag, FGameplayTag ActionName);

	void NativeInputReleased(FGameplayTag AbilityTag, FGameplayTag ActionName);
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerNativeInputReleased(FGameplayTag AbilityTag, FGameplayTag ActionName);
	virtual void ServerNativeInputReleased_Implementation(FGameplayTag AbilityTag, FGameplayTag ActionName);
	virtual bool ServerNativeInputReleased_Validate(FGameplayTag AbilityTag, FGameplayTag ActionName);
	void SendAbilityDeactivationMessage(FGameplayTag AbilityTag, FGameplayTag ActionName);
	//void BindAbilitiesToInputComponent(UInputComponent* InputComponentIn, )

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Ability"), Category = "Game Ability System")
		void BP_AddAbility(TSubclassOf<class UGAAbilityBase> AbilityClass, FGameplayTag ActionName);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Remove Ability"), Category = "Game Ability System")
		void BP_RemoveAbility(FGameplayTag TagIn);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Get Ability By Tag"), Category = "Game Ability System")
		UGAAbilityBase* BP_GetAbilityByTag(FGameplayTag TagIn);
	/*
		Should be called on server.
		Adds new ability to ActiveAbilities;
	*/
	void NativeAddAbilitiesFromSet(TSubclassOf<class UGAAbilitySet> AbilitySet);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Add Abilities From Set"), Category = "Game Ability System")
	void BP_AddAbilitiesFromSet(TSubclassOf<class UGAAbilitySet> AbilitySet);

	bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
protected:
	void InitializeInstancedAbilities();
	UGAAbilityBase* InstanceAbility(TSubclassOf<class UGAAbilityBase> AbilityClass, FGameplayTag ActionName = FGameplayTag());
	/*
		Messagin implementation for applying effects from multiple threads (in case 
		at some beatyfull day UObject will be able to exist on any thread), and from single thread.

		Implementation is based on FMessageEndpoint.
	*/

};



