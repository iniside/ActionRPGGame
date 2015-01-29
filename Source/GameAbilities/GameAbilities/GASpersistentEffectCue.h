#pragma once
#include "GASPersistentEffectCue.generated.h"

/*
	Actor which is can be used to controll persistent cue on level. For example if ability
	is channeled over time like AoE effect, which exist somewhere in level.

	It can be also used to spawn any other kind of effect, though for most cases
	it's probabaly more efficient to simply spawn one off componenets.
*/

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMEABILITIES_API AGASPersistentEffectCue : public AActor
{
	GENERATED_UCLASS_BODY()
public:

	//UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category = "Info")
	//class AGASAbility* OwningAbility;

	/*
		Who Instigated this cue.
	*/
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Info")
		AActor* CueInstigator;

	/*
		Initialize effect, but does not play it. Use it setup your properties.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Ability System") //temporary for testing.
		virtual void InitializeEffectCue();


	void BindCastDelegates();
	void BindPreparationDelegates();
	/*
		Use to play effect (particle effects, sounds, animations etc).
	 */
	UFUNCTION()
		virtual void ExecuteEffect();

	/*
		Helper to do cleanup if ability state ends, so does this actor.
	*/
	UFUNCTION()
		virtual void DestroyEffectCue();


	UFUNCTION(BlueprintImplementableEvent)
		void OnEffectExecuted();
};
