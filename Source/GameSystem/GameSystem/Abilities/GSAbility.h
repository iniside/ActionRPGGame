#pragma once
#include "GASAbility.h"

#include "../Cues/GSCueTypes.h"
#include "../Cues/IGSCue.h"

#include "GameplayTagContainer.h"

#include "GAGlobalTypes.h"

#include "IGESEffectManager.h"
#include "GSAbility.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FGSOnSetWeaponsForAbility, class UGSItemWeaponInfo*, class UGSItemWeaponInfo*);

USTRUCT(BlueprintType)
struct FGSAbilityCost
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere)
		FGAAttribute Attribute;
	UPROPERTY(EditAnywhere)
		float Cost;
};

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API UGSAbility : public UGASAbility, public IIGESEffectManager, public IIGSCue
{
	GENERATED_UCLASS_BODY()
public:
	float deleteMe;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Info")
		TArray<FGSAbilityCost> AttributeCost;

	class UGESEffect* GetCachedEffect();
private:
	UPROPERTY()
	class UGESEffect* CachedEffect;

protected:
	/*
		Currently equiped weapons.
		1. Weapon must be equiped to use ability.
		2. It must right type of weapon for this particular ability.
		3. At least one weapon must meet  criteria, unless ability specifies otherwise
		4. Lack of weapon is considere bare hands.
	*/
	UPROPERTY()
	class UGSItemWeaponInfo* CurrentLeftWeapon;
	UPROPERTY()
	class UGSItemWeaponInfo* CurrentRightWeapon;

public:
	FGSOnSetWeaponsForAbility OnSetWeaponsForAbility;
public:
	//set everytime weapon changes!
	inline void SetCurrentLeftWeapon(class UGSItemWeaponInfo* CurrentLeftWeaponIn) { CurrentLeftWeapon = CurrentLeftWeaponIn; };
	inline void SetCurrentRightWeapon(class UGSItemWeaponInfo* CurrentRightWeaponIn) { CurrentRightWeapon = CurrentRightWeaponIn; };
private:
	UFUNCTION()
		void SetWeaponForAbility(class UGSItemWeaponInfo* LeftWeaponIn, class UGSItemWeaponInfo* RightWeaponIn);
public:
	//virtual bool CheckIfCanUseAbility() override;

	FGSOnCueActionStarted OnCueActionStarted;
	FGSOnCueActionEnded OnCueActionEnded;

	/** IIGSCue overrides */
	virtual FGSOnCueActionStarted& GetOnCueActionStarted() override { return OnCueActionStarted; };
	virtual FGSOnCueActionEnded& GetOnCueActionEnded() override { return OnCueActionEnded; };
	/** IIGSCue overrides */

	/** AGASAbility overrides */
	//virtual void OnRep_CastEnded() override;
	//virtual void OnRep_CastStarted() override;
	/** AGASAbility overrides */

	//FVector GetSocketLocation(FName SocketNameIn) override;
	//virtual FVector GetTraceStartLocation() override;

	void SetOnSetWeaponsforability(class UGSItemWeaponInfo* LeftWeaponIn, class UGSItemWeaponInfo* RightWeaponIn);
protected:
	/**
	 *	Subtract cost of all attributes if that were needed to cast this ability.
	 */
	void SubtractAttributeCost();
};
