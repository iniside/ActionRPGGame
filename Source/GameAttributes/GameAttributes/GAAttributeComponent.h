#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffectCueGlobals.h"
#include "GAAttributesBase.h"
#include "GAGameEffect.h"
#include "GAAttributeComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("AttributeComponent"), STATGROUP_AttributeComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentApplyEffect"), STAT_ApplyEffect, STATGROUP_AttributeComponent, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentModifyAttribute"), STAT_ModifyAttribute, STATGROUP_AttributeComponent, );

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAOnAttributeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeModifed, const FGAModifiedAttribute&, attr);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGAGenericEffectDelegate, const FGAGameEffectHandle&, Handle, const FGameplayTagContainer&, Tags);

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

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEATTRIBUTES_API UGAAttributeComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Test")
		TArray<FGAGameEffectModifier> ModifierTest;

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
		FGACueContainer ActiveCues;
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

	UPROPERTY(ReplicatedUsing = OnRep_AttributeChanged)
		FGAModifiedAttributeData ModifiedAttribute;
	UFUNCTION()
		void OnRep_AttributeChanged();
	UPROPERTY(BlueprintAssignable, Category = "Game Attributes")
		FGAOnAttributeModifed OnAttributeModifed;


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
	FGAGameEffectContainer GameEffectContainer;

	template<typename T>
	T* GetAttributes()
	{
		return CastChecked<T>(DefaultAttributes);
	}

	UFUNCTION(BlueprintCallable, Category = "Test")
		void GetAttributeStructTest(FGAAttribute Name);

	virtual void InitializeComponent() override;

	virtual void UninitializeComponent() override;
public:
	/////////////////////////////////////////////////
	//////////// EFFECTS HANDLING

	FGAEffectHandle ApplyEffectToSelf(const FGAGameEffect& EffectIn, 
		const FGAGameEffectHandle& HandleIn);
	FGAEffectHandle ApplyEffectToTarget(const FGAGameEffect& EffectIn, 
		const FGAGameEffectHandle& HandleIn);

	FGAGameEffectHandle MakeGameEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn,
		const FGAEffectContext& ContextIn);

	void ApplyEffectForDuration(const FGAGameEffectHandle& HandleIn);

	/* Have to to copy handle around, because timer delegates do not support references. */
	void ExecuteEffect(FGAGameEffectHandle HandleIn);
	/* ExpireEffect is used to remove existing effect naturally when their time expires. */
	void ExpireEffect(FGAGameEffectHandle HandleIn);
	/* RemoveEffect is used to remove effect by force. */
	void RemoveEffect(FGAGameEffectHandle& HandleIn);
	void InternalRemoveEffect(FGAGameEffectHandle& HandleIn);

	void ApplyInstacnedEffectToSelf(class UGAEffectInstanced* EffectIn);
	void ApplyInstancedToTarget(class UGAEffectInstanced* EffectIn);

	void RemoveInstancedFromSelf(class UGAEffectInstanced* EffectIn);
	/* Never call it directly. */
	float ModifyAttribute(FGAEffectMod& ModIn);
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
		void MulticastApplyEffectCue(FGAGameEffectHandle EffectHandle, TSubclassOf<UGAEffectCue> EffectCue, FGAEffectCueParams CueParams);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExecuteEffectCue(FGAGameEffectHandle EffectHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRemoveEffectCue(FGAGameEffectHandle EffectHandle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateDurationCue(FGAGameEffectHandle EffectHandle, float NewDurationIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdatePeriodCue(FGAGameEffectHandle EffectHandle, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastUpdateTimersCue(FGAGameEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastExtendDurationCue(FGAGameEffectHandle EffectHandle, float NewDurationIn);

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

	void ModifyAttributesOnSelf(const FGAAttributeData& EvalData, const FGAEffectContext& Context, 
		const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn);
	void ModifyAttributesOnTarget(const FGAAttributeData& EvalData, const FGAEffectContext& Context, 
		const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn);
	
	//////Accessors to various functions inside DefaultAttributes;
	inline FGAAttributeBase* GetAttribute(const FGAAttribute& Name)
	{
		return DefaultAttributes->GetAttribute(Name);
	}
	
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
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;
private:
	class IIGAAttributes* AttributeInterface;
};



