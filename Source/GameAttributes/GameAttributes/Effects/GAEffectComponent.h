#pragma once
#include "GameplayTagContainer.h"
#include "../GAGlobalTypes.h"
#include "GAEffectTypes.h"
#include "GAEffectComponent.generated.h"

/*
	Provide two types of effects!
	1. Instanced Effects, which are essentialy UObjects with event graphs, they are instanced
	per application, can be created using blueprints. Useful for more complex tasks, like,
	If this target is moving, and shook head, apply damage, and this other effect.
	These types of tasks needs to be either coded in C++ (not ideal) or, in blueprintgraph.

	2. Simple (non instanced) effects. These effects, are using raw data from either data table
	or simple UObject asset. They don't do anything fancy. They just take raw information
	(attribute changes), and simply apply it to target. Since, they should be really cheap,
	we could spam thousands of them. Ideal application, for something like GW2 conditions,
	though with bit simplified (ie. no real time updates, of damage from source attribute changes).
	These effects, can't also have any logic build in using blueprints. They just change attributes,
	for the time they are active.

	3. For the sake of simplicity (??) all attribute modifications, should be routed trough effect
	component/effect objects. This way, we can give active effects, chance to affect incoming effects.
	(yey!).
*/
/*
	Continuing from above...

	When simple effect, is appilied we need to calculate it's mod (magnitued, whatever),
	we will do it based on:
	1. Base Value provided by, object who apply effect (??).
	2. Attribute value (we will sellect which attribute).
	3. We must decide how this attribute will modify base value (add, multiply, subtract, etc).
	4. Should multiple attributes affect calculation (in order of being added, not in Math order).
	(so here subtraction can happen before multiplication, just sayin).
	5. WE can also add coefficients. Idk if that has any real point...

	From this data, final attribute mod, is calculated on each tick of effect. That's the base level.

	1. Should we apply, any mods (like bonus to fire damage), here or it should be deffered
	until the damage will be appilied in AttributeComponent ?
*/

USTRUCT(BlueprintType)
struct FGAEffectData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	/** The 'Name' column is the same as the XP Level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
		float Level;
	/** XP to get to the given level from the previous level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
		FGAAttribute Attribute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LevelUp)
		TAssetPtr<UTexture> Asset;

	FGAEffectData()
	{}
};
/*
	Represent handle to exactly one active effect.

	Handle is generated from name of class/object. For now.
	Since not all effects are going to be instanced, is kind of impossible to generate
	them from memory adress.
*/
USTRUCT()
struct GAMEATTRIBUTES_API FGAEffectHandle
{
	GENERATED_USTRUCT_BODY()
public:
	FName Handle;

	FName Tag;

	FGAEffectHandle()
	{};
	FGAEffectHandle(FName HandleIn, FName TagIn)
	{
		Handle = HandleIn;
		Tag = TagIn;
	};
};

/*
	Representing Single active effect.
*/
USTRUCT()
struct GAMEATTRIBUTES_API FGAActiveEffect
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TWeakObjectPtr<class UGAEffect> Effect;

	FGAEffectHandle Handle;
	/*
		Do we really need copy of spec.
		Or it would be better to just, initialize spec once
		and just obtain values from it ?
	*/
	FGAEffectSpec Spec;
	FTimerHandle DurationHandle;
protected:
	int32 CurrentPeriodCount;
	int32 PeriodCount;

	UPROPERTY()
		TWeakObjectPtr<class UGAEffectComponent> OwningEffectComp;
public:
	void ActivateEffect();

	void FinishEffect();

	void OnEffectInitialize();
	void OnEffectOngoing();
	void OnEffectEnded();

	//we should call it on all effects, when we add new effect.
	//or maybe just limit ourselves to effects, which have required tags.
	//in anycase calling this will give chance for already going effect
	//to do something with this effect before applying, if needed.
	void OnEffectAdded(FGAEffectSpec& EffectIn);
protected:
	void ExecuteEffectPeriod();
	void ExecuteEffectDuration();

	void ApplyAttributeMods();
	//if we modified attribute by value
	//we can opt to restore the mod value to attribute after we expired.
	void RestoreAttributeValue();
public:
	FGAActiveEffect()
	{
		CurrentPeriodCount = 0;
		PeriodCount = 0;
		Effect.Reset();
	}
};

/*
	Contains all active effects. Of specific Tag.
*/
USTRUCT()
struct GAMEATTRIBUTES_API FGAEffectContainer
{
	GENERATED_USTRUCT_BODY()
public:
	TMap<FName, FGAActiveEffect> ActiveEffectsMap;//class name, active effect
	FGAEffectContainer()
	{
		ActiveEffectsMap = TMap<FName, FGAActiveEffect>();
	};
};

USTRUCT()
struct GAMEATTRIBUTES_API FGAActiveEffectContainer
{
	GENERATED_USTRUCT_BODY()
	//TMap<FName, FGAEffectContainer> ActiveEffectsTagsMap; //tag, container
};
/*
	This component allows interaction with Effect system.
*/
UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEATTRIBUTES_API UGAEffectComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UGAEffectComponent(const FObjectInitializer& ObjectInitializer);

	/*
		The core idea is that that we add effects with the same, tags, to single container.

		One Effect - One Tag ?

		Ok. Concept is working.
	*/
	TMap<FName, FGAEffectContainer> ActiveEffectsContainerMap;
	//we need replication. Though, replicating effects (Objects), is pointless.
	//what we really need to replicate is when effects is added, and when it ends. We
	//we canclculcate remaining time on client.
	//and get cosmetic information, from whatever appilied this object

	FGAEffectContainer EffectContainer;


	virtual void InitializeComponent() override;
	UFUNCTION()
	void EffectDurationExpired(FGAEffectHandle HandleIn);

	void ReceiveEffect(class UGAEffect* EffectIn);

	bool ApplyEffectToSelf(FGAEffectSpec& SpecIn);
	
	bool ApplyEffectToTarget(FGAEffectSpec& SpecIn, AActor* TargetIn);


	void RemoveEffectFromSelf(const FGameplayTagContainer& TagsIn, int32 ToRemove);
	void RemoveEffectFromTarget(AActor* TargetIn, const FGameplayTagContainer& TagsIn, int32 ToRemove);
};
