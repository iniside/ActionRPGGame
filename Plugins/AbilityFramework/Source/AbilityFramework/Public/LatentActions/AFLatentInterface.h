#pragma once
#include "AFLatentInterface.generated.h"


struct FAFAttributeBase;
struct FGAEffectHandle;
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class ABILITYFRAMEWORK_API UAFLatentInterface : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IAFLatentInterface
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual void OnLatentTaskAdded(FName InstanceName, class UAFTaskBase* TaskIn) = 0;
	virtual void AddReplicatedTask(class UAFTaskBase* TaskIn) = 0;
	virtual void OnLatentTaskRemoved(class UAFTaskBase* TaskIn) = 0;

	virtual void OnLatentTaskActivated(class UAFTaskBase* TaskIn) = 0;
	virtual void OnLatentTaskDeactivated(class UAFTaskBase* TaskIn) = 0;

	virtual class UAFTaskBase* GetCachedLatentAction(FName TaskName) = 0;
};