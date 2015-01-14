// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GISInventoryBaseComponent.h"
#include "GSEquipmentComponent.generated.h"


UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSEquipmentComponent : public UGISInventoryBaseComponent
{
	GENERATED_UCLASS_BODY()
public:
	/*
		List of currently equiped items.
	*/
	UPROPERTY()
		TArray<class UGSItem*> EquipedItems;
protected:
	UPROPERTY(EditAnywhere, Category = "Attachment Sockets")
		FName  LeftBackSocketName;
public:
	virtual void InitializeComponent() override;


	void EqiupItem(class UGSItem* ItemIn);

	void UnEqiupItem(class UGSItem* ItemIn);
	/*
		Equips item. Which means it will be attached/replcaed on owner of this component.

		@param MeshIn - mesh to attach to/set on skeletal mesh component.
		@param ComponentNameIn - name of component to which we will attach/Set

		It's temporary function used to figure out basic interface
		setting skeltal mesh, is probabaly last step we want to make.
	*/
	UFUNCTION(NetMulticast, Reliable)
		void SetSkeletalMesh(USkeletalMesh* MeshIn, FName ComponentNameIn);

	UFUNCTION(NetMulticast, Reliable)
		void RemoveSkeletalMesh(FName ComponentNameIn);

	UFUNCTION()
		void AttachActorTo(AActor* ActorIn);
private:
	class IIGSEquipment* EquipInt;
};



