#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributeBase.h"
#include "GAEffects.h"
#include "GAAttributesBase.h"
#include "GAAttributeComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("AttributeComponent"), STATGROUP_AttributeComponent, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentApplyEffect"), STAT_ApplyEffect, STATGROUP_AttributeComponent, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("AttributeComponentModifyAttribute"), STAT_ModifyAttribute, STATGROUP_AttributeComponent, );

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAOnAttributeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeModifed, const FGAModifiedAttribute&, attr);

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

	FGACountedTagContainer AppliedTags;
	
	FGAActiveEffectContainer ActiveEffects;

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

	UPROPERTY(ReplicatedUsing = OnRep_AttributeChanged, RepRetry)
		TArray<FGAModifiedAttribute> ModifiedAttribute;
	UPROPERTY()
		FGAOnAttributeModifed OnAttributeModifed;

	UFUNCTION()
		void OnRep_AttributeChanged();
	
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
	FGAEffectHandle ApplyEffectToSelf(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);
	/*
		As matter of fact I'm no sure if apply to target should either.
	*/
	FGAEffectHandle ApplyEffectToTarget(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);

	FGAEffectInstant MakeOutgoingInstantEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);
	void MakeOutgoingPeriodicEffect(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);

	void EffectExpired(const FGAEffectHandle& HandleIn);


	UFUNCTION(BlueprintCallable, Category = "Attribute Tests")
		FGAEffectHandle ApplySelfEffect(AActor* Target, APawn* Instigator,
		UObject* Causer, FGAEffectSpec SpecIn);

protected:
	void ExecuteModifiers(FGAAttributeData& ModifierIn, const FGameplayTagContainer& EffectTags, 
		const FGAEffectContext Context);

	//////////// EFFECTS HANDLING
	/////////////////////////////////////////////////

public:
	/////////////////////////////////////////////////
	//////////// ATTRIBUTES HANDLING
	/*
		I'm not entirely sure if this should be here. Just sayin.
	*/
	//FGAOnAttributeUpdated OnAttributeUpdated;

	/*
		Two functions, They will allow to apply any static numerical mods from player who
		initiated attribute change, and from player who will be affected by change.

		Mods will be appiled by small objects, and changed against tags.
		For example there might be physical armor mod, which will apply changes only
		to attributes tagged as Damage.Physical and only if you are reciving change, not causing it.
	*/
	void ModifyAttributesOnSelf(FGAAttributeData& EvalData, const FGAEffectContext& Context, 
		const FGameplayTagContainer& EffectTags, FGAEffectHandle& HandleIn);
	void ModifyAttributesOnTarget(FGAAttributeData& EvalData, const FGAEffectContext& Context, 
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



