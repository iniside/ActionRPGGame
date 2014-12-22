#pragma once
#include "GameplayTagContainer.h"
#include "GESEffectComponent.generated.h"

/*
	It's not going to look like this. I will probabaly wrap it into struct
	The genral idea we call event, when we apply effect, to give other effects chance to modify newly
	appiled effect.
*/
//actually I don't need them to be multicast delegates..
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGESOnEffectOutgoing, class UGESEffect*, EffectIn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGESOnEffectIncoming, class UGESEffect*, EffectIn);

/*
	I recived this effect, I will do nothing about it.

	delegate, which could be useful, for displaying information about effects in UI.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGESOnEffectAppiled, class UGESEffect*, EffectIn);

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGESOnEffectImmunity, bool&, bIsImmune, class UGESEffect, EffectIn);

DECLARE_DELEGATE_RetVal_OneParam(bool, FGESOnCheckImmunity, class UGESEffect*);

/*
	Encapsulate tag count. If new tag is added to TMap, and it have the same anme
	as existing key, instead counter in in this struct is increased.

	If Tag is removed and TagCount > 0, Decrement TagCount by 1
	If Tag is removed and TagCount reached 0 remove everything including key.
*/
USTRUCT()
struct FTagCounter
{
	GENERATED_USTRUCT_BODY()
public:
	int32 TagCount;

	FTagCounter()
	{
		TagCount = 0;
	};
};
/*
	I probabaly should add another component, which would allow spawning and caching of effects.
	Or UObject, which would default subobject. Not really important.

	What is important is that trough this object effect would be created in other objects, 
	and cached. We wouldn't need to construct new effect every time it is needed,
	we would just get the cached one, and update it's properties.

	Sounds logical eh ?
*/
/*
This component allows interaction with Effect system.
*/
UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEEFFECTSYSTEM_API UGESEffectComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/* temporary workaround, hopefully. */
	UPROPERTY(EditAnywhere, Category = "Effect")
		TArray<TSubclassOf<class UGESEffect>> StaticEffectClasses;

	/*
		Effects in both arrays are going to be executed exactly in the order
		as they are in array. So if several consecutive effects apply
		different numeric mods, to some value, it will modified by all effects
		in order of them in array.
	*/
	/*
		Static effects are always present on this component and this actor.
		They might not be active. 
		Because of that their indexes in array are also static.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
		TArray<class UGESEffect*> StaticEffects;
	
	/*
		List of periodic effects, contained by this component. This list can change
		during gameplay as other actors/objects might add new effects here, and after their time
		expires, they are removed from array and destroyed.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		TArray<class UGESEffect*> PeriodicEffects;
	
	/*
		We have two events. One for Outgoing effets, and one for incoming.
		Problem ? They are all called at the same time.
		Expected behaviour ?
		When I make effect I call Only Outgoing on me, since the effect
		is outgoing from me.

		When I receive effect I call only incoming, since effect is incoming to me.

		So I need a way to determine from where to where effect is going.

		So effects should do it like this, the call only Outgoing Event, on origin component,
		and only Incoming event on target component.

		Either way. I don't like how this work. To much space for errors.

		But I need effects to be able to modify other effects.
	*/
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGESOnEffectOutgoing OnEffectOutgoing;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGESOnEffectIncoming OnEffectIncoming;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGESOnEffectAppiled OnEffectAppiled;

	FGESOnCheckImmunity OnCheckImmunity;

	
	//rollout custom map, with immunity tags. Let's see how this will hold off.
	//serializing this map is probabaly beyond the point.
	//it doesn't need to be replicated
	//it will hold ALL immunity tags, including dynamic effects
	//and static effects, will be serialized as TArray
	//and in case of separate serialization for this TMap, I would have to use another TArray<>
	//pointless!
	//or maybe just use int32 here...
	TMap<FName, FTagCounter> ActiveEffectsTagsMap;

	TMap<FName, FTagCounter> ActiveImmunityTagsMap;

	virtual void InitializeComponent() override;

	void ReceiveEffect(class UGESEffect* EffectIn);

	//central function for applying effects.
	/**
	 *	What Should it do ? High level general concept.
	 *	1. We need to check if effect can be appiled to us.
	 *	1a. For example we can have immunity for effects of certain type.
	 *	This immunity can be permament, and that's easy part.
	 *	
	 *	1b. We can have other effects on us, which apply immunity to other effects.
	 *	These effects can be static (as they will be on us for entire game duration)
	 *	,or they can be temporary.
	 *	Worst case scenario is we would have to iterate over all active effects, and check
	 *	if they apply immunity! BADD!. Best option would be to call event, which would return
	 *	value. If at least one effect return true (immune), they we just stop at this point
	 *	And don't apply effect.
	 *	
	 *	1c. Checking is based on tags. 
	 *	Aggregate all incoming tags, into single container.
	 *	Check against this container on this component.
	 *	If effect is removed, remove it's tags from aggregator container. Might work.
	 *	This actually doesn't work. I forgot that TMap must have unique keys...
	 *	But I still want to leverage this general idea, as it is still, better that brute force
	 *	iteration over array and comparing fnames in loop. With, 10-20 entries it might not be issue.
	 *	with 200+, it's to much.
	 *
	 *	2. W need to evaluate effect. For example apply modifiers from other effects. Which are on us.
	 *	2a. But for that matter what about modifiers from whoever caused this effect ?
	 *
	 *	Important. This all should be boiled down to single Blueprint Exposed Function. However it
	 *	will look.
	 */
	void ApplyEffectToSelf(class UGESEffect* EffectIn);
	
	void ApplyEffectToTarget(class UGESEffect* EffectIn, AActor* TargetIn);
};
