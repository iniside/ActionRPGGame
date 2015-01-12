#pragma once
#include "IGSEquipment.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMESYSTEM_API UIGSEquipment : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGSEquipment
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual void SetSkeletalMesh(USkeletalMesh* MeshIn, FName ComponentNameIn){};

	virtual void RemoveSkeletalMesh(FName ComponentNameIn) {};

	virtual void AttachActor(AActor* ActorIn, FName SocketNameIn = NAME_None){};

	virtual USkeletalMeshComponent* GetSkeletalMeshComponentByName(FName NameIn) { return nullptr; };
};