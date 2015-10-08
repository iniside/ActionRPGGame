#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.h"
#include "GAEffectCueTypes.h"
#include "GAAttributesBase.h"
#include "GAAttributeComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("AttributeComponent"), STATGROUP_AttributeComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentApplyEffect"), STAT_ApplyEffect, STATGROUP_AttributeComponent, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentModifyAttribute"), STAT_ModifyAttribute, STATGROUP_AttributeComponent, );

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAOnAttributeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeModifed, const FGAModifiedAttribute&, attr);

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
	
	UPROPERTY(ReplicatedUsing=OnRep_ActiveEffects)
		FGAActiveEffectContainer ActiveEffects;
	UFUNCTION()
		void OnRep_ActiveEffects();

	UPROPERTY(ReplicatedUsing = OnRep_ActiveCues)
		FGAActiveEffectCuesContainer ActiveCues;
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

	
	UFUNCTION(BlueprintCallable, Category = "Test")
		void GetAttributeStructTest(FGAAttribute Name);

	virtual void InitializeComponent() override;

	virtual void UninitializeComponent() override;
public:
	/////////////////////////////////////////////////
	//////////// EFFECTS HANDLING

	/*
		Apply to self probabaly shouldn't take Spec.
	*/
	FGAEffectHandle ApplyEffectToSelf(TSubclassOf<class UGAEffectSpecification> SpecIn, 
		const FGAEffectContext& Context, const FName& EffeftName);
	/*
		As matter of fact I'm no sure if apply to target should either.
	*/
	FGAEffectHandle ApplyEffectToTarget(TSubclassOf<class UGAEffectSpecification> SpecIn, 
		const FGAEffectContext& Context, const FName& EffeftName);

	FGAEffectInstant MakeOutgoingInstantEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);
	void MakeOutgoingPeriodicEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);

	void EffectExpired(const FGAEffectHandle& HandleIn);

	void EffectRemoved(const FGAEffectHandle& HandleIn);

	UFUNCTION(BlueprintCallable, Category = "Attribute Tests")
		FGAEffectHandle ApplySelfEffect(AActor* Target, APawn* Instigator,
		UObject* Causer, FGAEffectSpec SpecIn);

	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		TArray<FGAEffectUIData> GetEffectUIData();
	/*
		Get Last Index of effect for UI display.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		int32 GetEffectUIIndex();

	UFUNCTION(BlueprintCallable, Category = "Game Attributes | UI")
		FGAEffectUIData GetEffectUIDataByIndex(int32 IndexIn);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastEffectCueExpired(int32 Handle);

	UFUNCTION(NetMulticast, Unreliable)
		void MulticastRemoveEffectCue(int32 Handle);

	/* 
		int param, is temporary work around, until I learn, how to tell engine, how
		it should serialize custom data types for replication
	*/
	void RemoveEffectCue(int32 Handle);

	void ApplyEffectCue(int32 Handle);
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



