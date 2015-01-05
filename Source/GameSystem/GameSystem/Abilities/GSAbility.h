#pragma once
#include "GASAbility.h"

#include "GAGlobalTypes.h"

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
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability Info")
		TArray<FGAAttribute> AttributeCost;

	class UGESEffect* GetCachedEffect();
private:
	UPROPERTY()
	class UGESEffect* CachedEffect;


public:
	virtual bool CheckIfCanUseAbility() override;

protected:
	/**
	 *	Subtract cost of all attributes if that were needed to cast this ability.
	 */
	void SubtractAttributeCost();
};
