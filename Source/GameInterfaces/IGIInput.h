#pragma once
#include "IGIInput.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEINTERFACES_API UIGIInput : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGIInput
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual void InputPressed(){};
	virtual void InputReleased(){};

	virtual void InputAltPressed(){};
	virtual void InputAltReleased(){};
};