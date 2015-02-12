#pragma once
#include "GAGlobalTypes.h"
#include "GAAttributesBase.h"
#include "GAAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGAOnAttributeChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGAOnAttributeModifed, const FGAModifiedAttribute&, attr);

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEATTRIBUTES_API UGAAttributeComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Set attribute which will be considered for indicating whether or not actor is dead.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		FGAAttribute DeathAttribute;

	/*
		TagName, number of tags. If Effect, provide immunity
		and such tag exist, increment value.

		If tag is removed decrement if value 0, remove entry, all togather.
		??
	*/
	TMap<FName, int32> ImmunityTags;

	FGACountedTagContainer AppliedTags;
	
	FGAActiveEffectContainer ActiveEffects;
	
	/*
		Tags, which are  currently applied by effects to me.
	*/
	//TMap<FGameplayTag, int32> AppliedTags;
	
	UPROPERTY(EditAnywhere, Category = "Attribute Mods")
		TArray<TSubclassOf<class UGAAttributeMod> > AttributeMods;
	
	UPROPERTY(VisibleAnywhere, Category = "Attribute Mods")
		TArray<class UGAAttributeMod*> AttributeModsObj;

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

	/////////////////////////////////////////////////
	//////////// EFFECTS HANDLING
	bool ApplyEffectToSelf(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);
	bool ApplyEffectToTarget(const FGAEffectSpec& SpecIn, const FGAEffectContext& Context);

	void EffectExpired(FGAEffectHandle& HandleIn);
	//////////// EFFECTS HANDLING
	/////////////////////////////////////////////////


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
	void ModifyAttributesOnSelf(TArray<FGAEvalData>& EvalData, const FGAEffectContext& Context, const FGAEffectHandle& HandleIn);
	void ModifyAttributesOnTarget(TArray<FGAEvalData>& EvalData, const FGAEffectContext& Context, const FGAEffectHandle& HandleIn);
	
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



