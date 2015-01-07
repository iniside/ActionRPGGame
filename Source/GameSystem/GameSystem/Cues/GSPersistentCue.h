#pragma once
#include "GSPersistentCue.generated.h"

/*
	Actor which is can be used to controll persistent cue on level. For example if ability
	is channeled over time like AoE effect, which exist somewhere in level.

	It can be also used to spawn any other kind of effect, though for most cases
	it's probabaly more efficient to simply spawn one off componenets.
*/

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSPersistentCue : public AActor
{
	GENERATED_UCLASS_BODY()
public:

	virtual void Tick(float DeltaSeconds) override;

	/*
		Needs way to dynamically update life time, when CueInstigator life time changes.
		For example channeled ability has extended duration during casting,
		but post this actor created.
	*/
	/**
	 *	Maximum amount of time, this cue will persist in level after spwaning.
	 *  if <= 0 it will exist as long as CueInstigator performing action.
	 *	set higher if you are using this to spawn effects which should presist
	 *	bit longer like explosion and smoke. 
	 */
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn), Category = "Info")
		float LifeTime;

	/*
		Who Instigated this cue.
	*/
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Info")
		AActor* CueInstigator;

	/*
		Initialize effect, but does not play it. Use it setup your properties.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game System|Cues") //temporary for testing.
		virtual void InitializeCue();

	/*
		Use to play effect (particle effects, sounds, animations etc).
	 */


	/*
		Helper to do cleanup if ability state ends, so does this actor.
	*/
	UFUNCTION()
		virtual void DestroyCue();


	UFUNCTION(BlueprintImplementableEvent)
		void OnEffectExecuted();

private:
	class IIGSCue* CueInt;
	float CurrentLifeTime;
};
