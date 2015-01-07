#pragma once
#include "GASAbility.h"

#include "../Cues/GSCueTypes.h"
#include "../Cues/IGSCue.h"

#include "GAGlobalTypes.h"

#include "IGESEffectManager.h"
#include "GSAbility.generated.h"

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSAbility : public AGASAbility, public IIGESEffectManager, public IIGSCue
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

	FGSOnCueActionStarted OnCueActionStarted;
	FGSOnCueActionEnded OnCueActionEnded;

	/** IIGSCue overrides */
	virtual FGSOnCueActionStarted& GetOnCueActionStarted() override { return OnCueActionStarted; };
	virtual FGSOnCueActionEnded& GetOnCueActionEnded() override { return OnCueActionEnded; };
	/** IIGSCue overrides */

	/** AGASAbility overrides */
	virtual void OnRep_CastEnded() override;
	virtual void OnRep_CastStarted() override;
	/** AGASAbility overrides */

protected:
	/**
	 *	Subtract cost of all attributes if that were needed to cast this ability.
	 */
	void SubtractAttributeCost();
};
