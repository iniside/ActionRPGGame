#pragma once
#include "GWWeapon.h"
#include "GWWeaponRanged.generated.h"

/*
	Base class for all weapons.
	Mele and ranged weapons need separate classes (at least).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEWEAPONS_API AGWWeaponRanged : public AGWWeapon
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float Range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float ReloadTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float MagazineSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float Spread;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ReloadState;

	virtual void InputPressed() override;
	virtual void InputReleased() override;

	virtual void BeginFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginFire();

	virtual void EndFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndFire();

	virtual void BeginReload();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginReload();

	virtual void EndReload(); //that's probabaly not needed.
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndReload();


	UFUNCTION(BlueprintImplementableEvent)
		void OnFireBegin();
	UFUNCTION(BlueprintImplementableEvent)
		void OnFireEnd();
	//replication of cues.
	UFUNCTION()
		void OnRep_Shooting();
	UFUNCTION()
		void OnRep_ReloadBeign();
	UFUNCTION()
		void OnRep_ReloadEnd();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Weapon Firing"))
		void OnWeaponFiring();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload Begin"))
		void OnReloadBegin();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload End"))
		void OnReloadEnd();
	/*
		There will be subclass of weapon parts
		there are going to be different for each weapon type (bow doesn't have barrel, I think).
		In editor, those can be setup by using Instanced properties
		at runetime they are going to be set, trough class pointer, and then constructed
		using FinishCustomization()
	 */
	//UPROPERTY()
	//	TSubclassOf<WeaponPart> Part;
};
