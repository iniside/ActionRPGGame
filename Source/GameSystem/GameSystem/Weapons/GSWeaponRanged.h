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
	UPROPERTY(EditAnywhere, Category = "Left Hand")
		UAnimSequence* LeftHandIdle;

	UPROPERTY(EditAnywhere, Category = "Left Hand")
		UAnimSequence* LeftHandCombat;

	UPROPERTY(EditAnywhere, Category = "Left Hand")
		UAnimSequence* LeftHandMove;

	UPROPERTY(EditAnywhere, Category = "Right Hand")
		UAnimSequence* RightHandIdle;

	UPROPERTY(EditAnywhere, Category = "Right Hand")
		UAnimSequence* RightHandCombat;

	UPROPERTY(EditAnywhere, Category = "Right Hand")
		UAnimSequence* RightHandMove;

	UPROPERTY(EditAnywhere, Category = "Both Hands")
		UAnimSequence* BothHandsIdle;

	UPROPERTY(EditAnywhere, Category = "Both Hands")
		UAnimSequence* BothHandsCombat;

	UPROPERTY(EditAnywhere, Category = "Both Hands")
		UAnimSequence* BothHandsMove;

	UPROPERTY(EditAnywhere, Category = "Both Hands")
		UAnimMontage* BothHandsMontage;

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
