#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAEffectTypes.h"
#include "GAEffect.generated.h"
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



	Update:
	Effects are small generic objects, which should be be spawnedby other objects.
	Blueprint which sets effect, doesn't define what the effect is, it just defines what it does.

	For example. Instead of defining burning effect, which will be defined as Fire damage over time
	we instead should setup generic Damage Over Time effect.

	The for example in ability we setup two effects. One is Damage and one is OverTime.

	On Damage effect we add tag Damage.Burning, and set damage caused,
	On OverTime time effect we add effect to execute, period, and maximum ticks.

	Of course for sake of convience, we can create base effects, which have pre setup tags,
	and we can just setup damage in ability.
*/
/*
	1. Should I add generic acccess to effect properties trough reflection ? Like GetFloat, GetInt,
	etc ?
	2. Should effect have specialized extensions classes ? Like for calculation ? 
	What would these extensions do ?
	3. What is reponsibility for effect ?
	4. Should I create subclass for effect duration types ? (EffectInstant, EffectPeriodic, EffectInfinite).
	5. Should effect be able to live outside of EffectComponent ? (probabaly not).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEATTRIBUTES_API UGAEffect : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Actor which is target of effect;
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		FHitResult Target;
	
	/*
		Instigator of effect. Actor which instigated effect.
		For example if effect was appiled by projectile, which in turn was spawned by Character,
		then Character is Instigator and Projectile is Causer.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		APawn* Instigator;
	
	/*
		Object which caused effect to be appiled.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		UObject* Causer;
	
	/*
		I originated from this effect component.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
	class UGAEffectComponent* InstigatorEffectComponent;

	/*
		I will be appiled to this effect Component.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
	class UGAEffectComponent* TargetEffectComponent;


	UPROPERTY(EditAnywhere, Category = "Type")
		EGAEffectType EffectType;

	UPROPERTY(EditAnywhere, Category = "Type")
		EGAEffectStacking EffectStacking;

	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Periodic Effect")
		int32 PeriodCount;
	/*
		If, EffectType set as Periodic and PeriodCount > 0 it will be Duration
		of single period.

		If EffectType set as Duration, this will be treated as Duration of entire effect
		no periodic event will be called.
	*/
	UPROPERTY(EditAnywhere, meta = (ExposeOnSpawn), Category = "Periodic Effect")
		float Duration;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Effect Values")
		float Magnitude;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Mods")
		TArray<FGAAttribute> AttributeToMod;

	/**
	 *	I have this tag.
	 *	We will allow only one tag, here, since it's tag by which type of effect is indentified.
	 *	We don't care if this effect does damage (or what kind of damage), what we care about
	 *	is specified type of effect. Like Hex, Damge Over Time, Burning, Pertified, 
	 *	Enchatment, you got the idea.
	 *	If for example effect causes damage, this damage will have it's own tags, separate from
	 *	effect tags (though, it might inherit effects tags if so desired).
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Tags")
		FGameplayTag MyTag;

	UPROPERTY(BlueprintReadOnly, Category = "Tags")
		FGameplayTagContainer MagnitudeTags;

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

protected:
	FTimerHandle EffectPeriodTimerHandle;
	int32 CurrentPeriodCount;
public:
	virtual void Initialize();
	virtual void DestroyEffect();
	/*
		Called after effect is fully initialized.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Effect")
		void OnEffectInitialized();
	/**
	 *	Called if effect has been succeefuly appiled to target;
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Effect")
		void OnEffectAppiled();

	UFUNCTION(BlueprintImplementableEvent)
		void OnEffectPeriod();

	UFUNCTION()
		void CallEffectPeriod();
private:
	/*
		Workaround for the fact that BlueprintNativeEvent functions can't be void and being overriden
		in blueprint.

		And at the same time function returning value does not match with delegate signature.
	*/

};
