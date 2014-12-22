#pragma once
#include "GASAbility.h"
#include "IGESEffectManager.h"
#include "GSAbility.generated.h"
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMESYSTEM_API AGSAbility : public AGASAbility, public IIGESEffectManager
{
	GENERATED_UCLASS_BODY()
public:
	float deleteMe;

	class UGESEffect* GetCachedEffect();
private:
	UPROPERTY()
	class UGESEffect* CachedEffect;
};
