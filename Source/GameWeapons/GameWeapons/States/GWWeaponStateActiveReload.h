#pragma once
#include "GWWeaponState.h"
#include "GWWeaponStateActiveReload.generated.h"
/*
	I could, probabaly remake it with interface instead of within.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GWWeapon)
class GAMEWEAPONS_API UGWWeaponStateActiveReload : public UGWWeaponState
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds);
	virtual void BeginState(UGWWeaponState* PrevState);
	virtual void EndState();
	virtual void BeginActionSequence();
	virtual void EndActionSequence();
protected:
	UPROPERTY()
	class AGWWeaponRanged* CurrentWeapon;

	UFUNCTION()
		void EndWeaponReload();
	FTimerHandle ReloadTimerHandle;
};
