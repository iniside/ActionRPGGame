#pragma once
#include "IGISkeletalMesh.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEINTERFACES_API UIGISkeletalMesh : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGISkeletalMesh
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual USkeletalMeshComponent* GetMasterSkeletalMesh() { return nullptr; };
};