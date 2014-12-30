#pragma once
#include "IGASAbilities.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEABILITIES_API UIGASAbilities : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGASAbilities
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };
};