#pragma once
#include "IGTSocket.generated.h"

/*
	Simple interface to retrive socket location from actor.
	Or any other position by FName.
*/

UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMETRACE_API UIGTSocket : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGTSocket
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };
};