#pragma once
#include "GWWeaponState.h"
#include "GWWeaponStateSingleShot.generated.h"
/*
	Reaload weapon by one bullet at time.
	
	ReloadTime - indicates how long it will take load singe bullet.
	Total reaload time is BulletToReload * ReloadTime.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GWWeapon)
class GAMEWEAPONS_API UGWWeaponStateSingleShot : public UGWWeaponState
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

	bool bCanRefire;
	UFUNCTION()
		void SetCanRefireWeapon();

	FTimerHandle CanRefireTimerHandle;
};
