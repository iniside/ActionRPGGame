#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"

#include "AFTaskManager.generated.h"

UCLASS()
class ABILITYFRAMEWORK_API UAFTaskManager : public UObject
{
	GENERATED_BODY()

	static UAFTaskManager* Instance;

public:
	UAFTaskManager(const FObjectInitializer& ObjectInitializer);
	static UAFTaskManager* Get();
};
