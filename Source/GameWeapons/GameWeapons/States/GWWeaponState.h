#pragma once
#include "GWWeaponState.generated.h"
/*
	I could, probabaly remake it with interface instead of within.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GWWeapon)
class GAMEWEAPONS_API UGWWeaponState : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;

	virtual void Tick(float DeltaSeconds);
	virtual void BeginState(UGWWeaponState* PrevState);
	virtual void EndState();
	virtual void BeginActionSequence();
	virtual void EndActionSequence();
};
