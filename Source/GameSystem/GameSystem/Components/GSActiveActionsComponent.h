// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GSGlobalTypes.h"
#include "../GSWeaponEquipmentTypes.h"
#include "GISInventoryBaseComponent.h"
#include "GSActiveActionsComponent.generated.h"


UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSActiveActionsComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
protected:
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FName  LeftHandName;

	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FGSWeaponSocketInfo  LeftSocketInfo;

public:
	virtual void InitializeComponent() override;

	//refactoring into more meaningful names.
	void GetNextWeapon();
	void OnWeaponActive();



	void CopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerCopyNextItemFromOtherInventoryTab(class UGISInventoryBaseComponent* OtherIn, int32 OtherTabIndex, int32 TargetTabIndex, int32 TargetSlotIndex, EGSWeaponHand WeaponHandIn);

	virtual void OnActionCopiedFromOtherInventoryTab(class UGISItemData* DataIn, class UGISItemData* LastDataIn, EGSWeaponHand WeaponHandIn);

private:
	int32 LastLeftCopiedIndex;
	int32 LastRightCopiedIndex;
	class IIGSEquipment* EquipInt;

	UPROPERTY()
	class UGISItemData* CurrentLeftHandWeapon;
	UPROPERTY()
	class UGISItemData* CurrentRightHandWeapon;
}; 



