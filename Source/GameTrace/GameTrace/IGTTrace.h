#pragma once
#include "IGTTrace.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMETRACE_API UIGTTrace : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGTTrace
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual APawn* GetPawn() { return nullptr; };

	virtual APlayerController* GetPC() { return nullptr; };
};