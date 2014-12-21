#pragma once
#include "GameplayTagContainer.h"
#include "GESEffect.generated.h"
/*
	Base class for effect.

	Effect is object, which modify actor with EffectComponent (or something like that).

	Effects can be divided into three basic categories:

	1. Instant - effect is appiled to actor, modify actor properties, and is deactivated and then
	garbage collected.
	2. Duration - effect is appiled to actor for set duration, after which is removed.
	3. Infinite - effect will exist for entire duration of game. (or actor).


	The main idea behind effects is to extend them trough blueprints, the base class shouldn't assume
	any gameplay type behaviours, only allow to easily create them trough blueprint.

	How exactly will that look, I'm not 100% sure.

	Effects should also be able to modify other effects.
	It's more complex than simply modifying value on other effects.
	The simplest way to put how it works would be
	If(Enemy Player is affected by Burning)
	{
		Apply Effect Core
		Apply Slow
	}
	else
	{
		Apply EffectCore
	}

	While(slow is appiled)
	{
		Apply cripple effect on damage.
	}

	In other words effects should be able to respond to incoming and outgoing events.
	Apply other effects, when certain effects occure.

	There is no any preset methods of calculating any numerical mods. You have to implement them
	yourself in respective functions. Either using blueprint or C++.
*/
/*
	1. Should I add generic acccess to effect properties trough reflection ? Like GetFloat, GetInt,
	etc ?
	2. Should effect have specialized extensions classes ? Like for calculation ? 
	What would these extensions do ?
	3. What is reponsibility for effect ?
	4. Should I create subclass for effect duration types ? (EffectInstant, EffectPeriodic, EffectInfinite).
	5. Should effect be able to live outside of EffectComponent ? (probabaly not).
	6. 
	
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEEFFECTSYSTEM_API UGESEffect : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Actor which is target of effect;
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		AActor* Target;
	
	/*
		Instigator of effect. Actor which instigated effect.
		For example if effect was appiled by projectile, which in turn was spawned by Character,
		then Character is Instigator and Projectile is Causer.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		AActor* Instigator;
	
	/*
		Actor which cause effect to be appiled.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		AActor* Causer;
	
	/*
		I originated from this effect component.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
	class UGESEffectComponent* OutgoingEffectComponent;

	/*
		I will be appiled to this effect Component.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
	class UGESEffectComponent* IncomingEffectComponent;

	/**
	 *	I have these tags
	 */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer MyTags;
	/**
	 *	I require other effect to have these tags, so I can modify it.
	 */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RquiredTags;
	/**
	 *	I will remove other effects, with these tags, when I'm appiled.	
	 */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer RemoveEffectsWithTags;
	/**
	 *	I will apply immunity to effects with these tags, as long as I'm active.
	 */
	UPROPERTY(EditAnywhere, Category = "Tags")
		FGameplayTagContainer ImmunityTags;


	virtual void Initialize();

	/*
		Called after effect is fully initialized. Should be used, to execute any
		logic implemented in blueprint. In event graph.

		Possibly could be make override;
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect")
		void OnEffectInitialized();
	/**
	 *	Called if effect has been succeefuly appiled to target;
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect")
		void OnEffectAppiled();

	/*
		Modify effects which are originating from Me.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
		bool ModifyOutgoingEffect(class UGESEffect* EffectIn);
	/*
		Modify effects which are coming to Me.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
		bool ModifyIncomingEffect(class UGESEffect* EffectIn);
	/** 
	 * Implement this function, if you want this effect to apply immunities.
	 * use Tags from ImmunityTags and check them against MyTags of incoming effect.
	 */
	UFUNCTION()
		virtual bool CheckImmunity(class UGESEffect* EffectIn);

	/*
	 *	Called on component, when applyging this effect, to check
	 *	if this effect can be exectued and/or appiled.
	 */
	void CheckForImmunities(class UGESEffect* EffectIn);

	void ExecuteOutgoing();
	void ExecuteIncoming();
private:
	/*
		Workaround for the fact that BlueprintNativeEvent functions can't be void and being overriden
		in blueprint.

		And at the same time function returning value does not match with delegate signature.
	*/
	UFUNCTION()
		void OnOutgoingEffect(class UGESEffect* EffectIn);
	UFUNCTION()
		void OnIncomingEffect(class UGESEffect* EffectIn);
};
