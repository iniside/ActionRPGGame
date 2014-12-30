#pragma once
#include "GASAbilityAction.h"
#include "GASAbilityActionTrace.h"
#include "GASAbilityActionTrace_GroundSphere.generated.h"
/*
	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASAbilityActionTrace_GroundSphere : public UGASAbilityActionTrace
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "Configuration")
		TSubclassOf<AActor> DisplayHelperActor;

	UPROPERTY(EditAnywhere, Category = "Configuration")
		float Radius;

	/**/
	virtual void Tick(float DeltaSecondsIn) override;

	/**
	*	Call it to display cosmetic helpers.
	*/
	virtual void PreExecute() override;

	/**
	 *	Central function to execute current action.
	 */
	virtual void Execute() override;

	virtual void PostExecute() override;

protected:
	UPROPERTY()
		AActor* DisplayHelper;

};
