#pragma once
#include "GWWeaponState.h"
#include "GWWeaponStateActive.generated.h"
/*
	I could, probabaly remake it with interface instead of within.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GWWeapon)
class GAMEWEAPONS_API UGWWeaponStateActive : public UGWWeaponState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginState(UGWWeaponState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;
};
