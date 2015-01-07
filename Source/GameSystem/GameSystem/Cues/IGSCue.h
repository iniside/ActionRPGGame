#pragma once
#include "GSCueTypes.h"
#include "IGSCue.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMESYSTEM_API UIGSCue : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGSCue
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FGSOnCueActionStarted& GetOnCueActionStarted() = 0;

	virtual FGSOnCueActionEnded& GetOnCueActionEnded() = 0;
};