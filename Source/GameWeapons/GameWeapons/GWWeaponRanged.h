#pragma once
#include "GWWeapon.h"
#include "GWWeaponRanged.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FGWOnCurrentWeaponSpread, float);

USTRUCT(BlueprintType)
struct GAMEWEAPONS_API FGWRecoilInfo
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	 *	How much gun will move horizontally while fire is pressed.
	 *	Negative Left, positive right.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Horizontal")
		float HorizontalRecoilBase;
	/**
	 *	Maximum offset for horizontal recoil. Anything beyond that will be clamped to max
	 *	value.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Horizontal")
		float HorizontalRecoilMaximum;
	/**
	 *	Recoil Multiplier. How much (quickly) will gun reach it's maximum
	 *	recoil the formula is:
	 *	HorizontalRecoilBase = HorizontalRecoilBase * HorizontalRecoilMultiplier;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Horizontal")
		float HorizontalRecoilMultiplier;
	/**
	 *	How much gun will move vertically while fire is pressed.
	 *	Negative down, positive up.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float VerticalRecoilBase;
	/**
	 *	Maximum offset for horizontal recoil. Anything beyond that will be clamped to max
	 *	value.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float VerticalRecoilMaximum;
	/**
	 *	Recoil Multiplier. How much (quickly) will gun reach it's maximum
	 *	recoil the formula is:
	 *	VerticalRecoilBase = VerticalRecoilBase * VerticalRecoilMultiplier;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float VerticalRecoilMultiplier;
	/**
	 *	Random spread for bullets. Direction is spread is picked randomly.
	 *	This value indicates how far from weapon bullet can be offested.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float RandomSpreadBase;
	/**
	 *	Maximum amount of spread. Values higher than this will be clmaped.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float RandomSpreadMaximum;
	/**
	 *	Spread Multiplier. How much spread will be increased on each shot.
	 *	RandomSpreadBase = RandomSpreadBase * RandomSpreadMultiplier;
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recoil Vertical")
		float RandomSpreadMultiplier;

};

/*
	Base class for all weapons.
	Mele and ranged weapons need separate classes (at least).
*/
UCLASS(BlueprintType, Blueprintable, hideCategories=(Animation, Transform, Mesh, Physics, Collision, Clothing, Lighting, "Physics Volume", "Skeletal Mesh"))
class GAMEWEAPONS_API AGWWeaponRanged : public AGWWeapon
{
	GENERATED_UCLASS_BODY()
public:
	/*
		TODO:
		Refactor it into structs. Gonna be easier to handle.
	*/
	/*
		Indicates that HitInfo trace should be done every frame.
		Might be useful for displaying beam effects
		or targeting helpers, which need to be updated every frame.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		bool bTraceEveryTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float Range;
	/*
		How often weapon can refire. Either automatic, semi-automatic or single shot.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float FireRate;

	/*
		Reaload time. Either for single bullet or for entire magazine.
		Depends on reload State type.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float ReloadTime;

	//possibly move it to separate object ?
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MagazineSize;

	/*
		Maximum ammo this weapon can have.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 MaximumAmmo;
	/*
		Base spread for this weapon.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float BaseSpread;
	/*
		Maximum spread of this weapon. Used to clamp, CurrentSpread, so we won't get some ridiculous values..
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float MaximumSpread;
	/*
		If weapon fires continously, spread will increase by this multiplier, every time weapon shoot.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float SpreadMultiplier;
	/*
		Base reduce value of CurrentSpread, when weapon is not firing.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float SpreadReduce;
	/*
		Amount of bullets shoot in single shoot. Default 1.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 BurstSize;
	/*
		Separate from BurstSize. Shotgun can have high burst size, but still only one ammo cost.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		int32 AmmoCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		float MaximumChargeTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		FGWRecoilInfo RecoilConfig;
	/*
		Type of ammo, this weapon currently have.
		Ammo type indicates type of damage this weapon will deal.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Modifications")
		TSubclassOf<class UGWAmmo> TypeOfAmmo;


	FGWOnCurrentWeaponSpread OnCurrentWeaponSpread;
protected:
	UPROPERTY(BlueprintReadOnly, Category ="Ammo")
	class UGWAmmo* CurrentAmmo;
public:
	/*
		Type of reaload used by this weapon.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ReloadState;

	UPROPERTY(EditAnywhere, Category = "Cosmetics")
		UAnimMontage* FiringAnimation;

	UPROPERTY(EditAnywhere, Category = "Cosmetics")
		UAnimMontage* ReloadAnimation;

	virtual void Tick(float DeltaSeconds) override;

	/*
		Function used to execute fireing weapon.
		It will use CurrentAmmo, to apply damage to Target.

		@param TargetIn - Target to which ammo will hit.zx
	*/
	UFUNCTION(BlueprintCallable, Category = "Game Weapons")
		void FireWeapon(const FHitResult& TargetIn, float DamageIn, APawn* InstigatorIn);

	/*
		Mele weapons don't display crosshair ?
	*/
	bool bCanDisplayCrosshair;

protected:
	UPROPERTY(Replicated)
	float RemainingMagazineAmmo;
	UPROPERTY(Replicated)
	float RemaningAmmo;

	float CurrentSpread;
	bool CheckIfHaveAmmo();
	void SubtractAmmo();
	void CalculateReloadAmmo();
	bool CheckIfCanReload();

	float CurrentCharge;

	void ReduceSpreadOverTime();
	FTimerHandle ReduceSpreadOverTimeTimerHandle;

public:
	//UPROPERTY(BlueprintReadOnly, Catgory = "Game Weapon|Recoil")
		float CurrentHorizontalRecoil;
	//UPROPERTY(BlueprintReadOnly, Catgory = "Game Weapon|Recoil")
		float CurrentVerticalRecoil;
public:
	/*
		Set remaning ammo in magazine from external source. Ie. saved data.
	*/
	inline void SetRemainingMagazineAmmo(int32 RemainingAmmoIn){ RemainingMagazineAmmo = RemainingAmmoIn; };
	/*
		Get remaning ammo  in magazine . For example for saving ;).
	*/
	inline int32 GetRemainingMagazineAmmo() { return RemainingMagazineAmmo; };

	/*
		Set remaning total ammo from weapon in external source. Ie. saved data.
	*/
	inline void SetRemaningAmmofloat(int32 RemainingAmmoIn){ RemaningAmmo = RemainingAmmoIn; };
	/*
		Get remaning total ammo for this weapon for external source. For example for saving ;).
	*/
	inline int32 GetRemaningAmmo() { return RemaningAmmo; };

	inline float GetCurrentSpread(){ return CurrentSpread; };

	/*
		Calculates current weapon spread. Called on every shot.
	*/
	virtual void CalculateCurrentWeaponSpread();
public:
	/*
		Ranged weapons need special trace methods, which will account for spread.
		also spread should be calculcated based on how long weapon fire.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Targeting")
	class UGWTraceRangedWeapon* TargetingMethod;

	virtual void BeginPlay() override;

	virtual void InitializeWeapon() override;

	virtual void InputPressed() override;
	virtual void InputReleased() override;

	virtual void InputReload();

	/*
		Shoots weapons. Going to be called from client and server. Though there is no synchronization btween those two.
		And client is mainly using for cosmetics.
	*/
	virtual void ShootWeapon();
	virtual void ActionEnd() override;

	virtual void BeginFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginFire();

	virtual void EndFire();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndFire();

	/*
		Begin reloading weapon. for example start playing animation.
	*/
	virtual void BeginReload();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerBeginReload();
	/*
		Finish reloading. At this point it should be safe to add  ammo to magazine.
	*/
	virtual void EndReload(); //that's probabaly not needed.
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerEndReload();


	UFUNCTION(BlueprintImplementableEvent)
		void OnShoot();
	UFUNCTION(BlueprintImplementableEvent)
		void OnFireEnd();
protected:

	UFUNCTION()
		virtual void OnRep_Shooting();
	UPROPERTY(ReplicatedUsing = OnRep_ReloadBeign)
		int8 ReloadBeginCount;
	UFUNCTION()
		virtual void OnRep_ReloadBeign();
	UPROPERTY(ReplicatedUsing = OnRep_ReloadEnd)
		int8 ReloadEndCount;
	UFUNCTION()
		virtual void OnRep_ReloadEnd();

	virtual void OnRep_HitInfo() override;
public:
	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Weapon Firing"))
		void OnWeaponFiring(const FVector& Origin, const FVector& HitLocation);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload Begin"))
		void OnReloadBegin();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Reload End"))
		void OnReloadEnd();

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName = "On Weapon Charging"))
		void OnWeaponCharging();
private:
	class IIGISkeletalMesh* SkeletalMeshInt;

	bool bIsWeaponFiring;
	/*
		There will be subclass of weapon parts
		there are going to be different for each weapon type (bow doesn't have barrel, I think).
		In editor, those can be setup by using Instanced properties
		at runetime they are going to be set, trough class pointer, and then constructed
		using FinishCustomization()
	 */
	//UPROPERTY()
	//	TSubclassOf<WeaponPart> Part;
	/*
		Ammo type is type of damage which instant hit weapon will deal.
		Also it can cause special effects like freezing, immolation etc.
		Class pointer on purpose so it can be changed on runtime.
	*/

};
