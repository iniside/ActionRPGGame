#pragma once
#include "GASAbility.generated.h"
/*
	Could be UObject, replicated trough component.
	But is it worth fuss ?
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced)
class GAMEABILITIES_API AGASAbility : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	virtual void Tick(float DeltaSeconds) override;


	/**
	 *	State Machine Properties Begin
	 */
	/**
	*	State used when this ability is active
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* ActiveState;
	/**
	*	State used when ability is on cooldown.
	*/
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* CooldownState;
	/**
	 *	State usedwhen abiliti is activated.
	 */
	UPROPERTY(EditAnywhere, Instanced, Category = "Ability State")
	class UGASAbilityState* ActivationState;

	UPROPERTY()
	class UGASAbilityState* CurrentState;
	/**
	 *	State Machine Properties End
	 */

	/**
	 *	State Machine Function Begin
	 */
	void GotoState(class UGASAbilityState* NextState);

	inline void GotoActiveState()
	{
		GotoState(ActiveState);
	};
	/**
	 *	State Machine Function End
	 */

	/**
	 *	Hook it up to input.
	 */
	void InputActivate();

	void ActivateAbility();


	/**
	 *	Blueprint Events
	 */
	/**
	 *	Called when ability is Activated (ie. casting finished).
	 *	Make it overridable in blueprint ? Or just use it to start event graph ?
	 */
	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownEnded();
};
