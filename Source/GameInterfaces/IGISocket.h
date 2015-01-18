#pragma once
#include "IGISocket.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEINTERFACES_API UIGISocket : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGISocket
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };
};