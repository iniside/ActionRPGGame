#pragma once
#include "GAAbilityCue.generated.h"
/*
	Ability cues are object, which will spawn cosmetic effects, for abilities.

	Make it actor, or make them spawn actors and/or component ?

	If the former I would need to replicate this trough ability actor, which would
	own particular cue.
	///bla bla don't know if above is still relevelant.

	But. 
	AbilityCues will spawn only effects, sounds, etc at locations, which are available trough Pawn.
	Be it socket on weapon, skeleton, or direct pawn location.

	For spawning effects at arbitrary world location, one should use GACueActor.
	Though technically there is nothing stoping anyone from spawning PSC at world location, trough trace.

	Neither AbilityCue or CueActor will spawn effects on other actor/pawn, since they don't have information
	about target.

	Only Effects, can spawn Cues, which will add Cues at Other Actor/Target location.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GAAbilityBase)
class ABILITYFRAMEWORK_API UGAAbilityCue : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintPure, Category = "Game Abilities")
		APawn* GetPawnOwner() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownExpired();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityActivationStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityPeriod();

	//from ability anim notifies:

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityStartNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityEndNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotify();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateTick();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityNotifyStateEnd();
};
