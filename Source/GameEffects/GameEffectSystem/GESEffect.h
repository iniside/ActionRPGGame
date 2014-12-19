#pragma once
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

	virtual void Initialize();


	/*
		Modify effects which are originating from you.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
		bool ModifyOutgoingEffect(class UGESEffect* EffectIn);
	/*
		Modify effects which are coming to you.
	*/
	UFUNCTION(BlueprintNativeEvent, Category = "Effect")
		bool ModifyIncomingEffect(class UGESEffect* EffectIn);
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
