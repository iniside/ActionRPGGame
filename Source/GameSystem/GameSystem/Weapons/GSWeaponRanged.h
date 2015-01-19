#pragma once
#include "GWWeaponRanged.h"
#include "../GSWeaponEquipmentTypes.h"
#include "../GSGlobalTypes.h"
#include "GSWeaponRanged.generated.h"
/*
	Add interface IWeaponEquip, which would let each weapon decide
	how it want to be equiped
*/
UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSWeaponRanged : public AGWWeaponRanged
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "UI")
		UTexture2D* Icon;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
		EGSWeaponWield WieldType;

	UPROPERTY(EditAnywhere, Category = "Weapon Config")
		EGSWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* EquipedAnimation;

	/*
		New idle and combat animations.
		Idle is relaxed pose (gun down), combat is aimed pose.
		All animations are one frame.
	*/
	//asd
	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* LeftHandAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* RightHandAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* BothHandsAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* RelaxedLeftHandAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* RelaxedRightHandAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAnimSequence* RelaxedBothHandsAnimation;

	UPROPERTY(EditAnywhere, Category = "Weapon Animation")
		UAimOffsetBlendSpace* AimOffsetBlend;
	/**
	 *	List of possible sockets, to which this weapon can attach itself when equiped.
	 */
	UPROPERTY(EditAnywhere, Category = "Socket Attachments")
		TArray<FGSWeaponSocketInfo> SocketList;

	/*
		move equiping logic here in form of interface
		1. Weapon know of it's type.
		2. We have to check what are currently equiped weapons.
		3. Weapon must know in what hand it currently is.
	*/
};
