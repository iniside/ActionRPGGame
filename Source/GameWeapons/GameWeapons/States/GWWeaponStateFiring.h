#pragma once
#include "GWWeaponState.h"
#include "GWWeaponStateFiring.generated.h"
/*
	I could, probabaly remake it with interface instead of within.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GWWeapon)
class GAMEWEAPONS_API UGWWeaponStateFiring : public UGWWeaponState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginState(UGWWeaponState* PrevState) override;
	virtual void EndState() override;
	virtual void BeginActionSequence() override;
	virtual void EndActionSequence() override;
protected:
	void FireWeapon();
	UPROPERTY()
	class AGWWeaponRanged* CurrentWeapon;
	FTimerHandle FiringTimerHandle;
};
