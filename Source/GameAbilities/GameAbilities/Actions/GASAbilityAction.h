#pragma once
#include "GASAbilityAction.generated.h"
/*
	AbilityActions are generic (preferably C++) defined actions, which then can be added to ability and
	the should be activated from ability. 
	Then can perform tasks, like spawn tagetting helpers (splines, circles), spawn actors, 
	gather targeting data etc.

	Should they be activated automatically after ability is initialized, (it'e ability enterted in
	active state, which means it's ready to be fired and display helpers, but did not yet received input,
	or should designer in blueprint decide when to launch actions ?).
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASAbilityAction : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;
};
