#pragma once
#include "GameplayTagContainer.h"
#include "GESEffectManager.generated.h"

UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class GAMEEFFECTSYSTEM_API UGESEffectManager : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY()
	class UGESEffect* CachedEffect;
};
