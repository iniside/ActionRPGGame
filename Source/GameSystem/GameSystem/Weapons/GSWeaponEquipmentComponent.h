// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "../GSWeaponEquipmentTypes.h"
#include "GameplayTagContainer.h"
#include "GISInventoryBaseComponent.h"
#include "GSWeaponEquipmentComponent.generated.h"


UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSWeaponEquipmentComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
public:
	virtual void InitializeComponent() override;
	UFUNCTION()
		void AttachActorTo(AActor* ActorIn, FName& AttachedSocketNameOut, const TArray<FGSWeaponSocketInfo>& PossibleSockets);
public:
	/**
	 *	Contains all sockets, to which we can attach weapon.
	 *	Also contains information is something is attached to this socket.
	 *	Currently not used!
	 */
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		TArray<FGSWeaponSocketInfo> AttachmentSockets;

	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FName  LeftBackSocketName;
protected:
	class IIGSEquipment* EquipInt;
};



