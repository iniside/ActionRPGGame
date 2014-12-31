#pragma once
#include "GASAbilityCue.generated.h"
/*
	Ability cues are object, which will spawn cosmetic effects, for abilities.

	Make it actor, or make them spawn actors and/or component ?

	If the former I would need to replicate this trough ability actor, which would
	own particular cue.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASAbilityCue : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
};
