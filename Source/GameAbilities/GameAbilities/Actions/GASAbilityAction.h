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

	virtual void Tick(float DeltaSecondsIn);
	/*
		Called before Exectute.
		Wheen exactly will you call it is up to you. I assume it might be usefull to display
		cosmetic helpers first, and then to execute action (for example to gather
		data, which is under cosmetic helper).
	*/
	virtual void PreExecute();
	/*
		Called to execute current action.
	*/
	virtual void Execute();
	/*
		Executed after action has been exectued.
		Can be used for cleanup.
	*/
	virtual void PostExecute();
};
