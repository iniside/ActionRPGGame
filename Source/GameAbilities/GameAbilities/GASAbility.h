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

	/*
		Property here or in states ?
		CastTime, ChannelTime, period time (Ability pulse, every X seconds while active or while
		pressed)
	*/
	UPROPERTY()
		float MaxCastTime;

	UPROPERTY()
		float CooldownTime;
private:
	float CurrentCooldownTime;
	float CurrentCastTime;
public:
	inline float GetCooldownTime() { return CurrentCooldownTime; };

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		FText AbilityName;

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		UTexture2D* AbilityIcon;

	UPROPERTY(EditAnywhere, Category = "Ability Cosmetics")
		FText AbilityDescription;
	/*
		Does this ability have preparation stage. Ie. in AoE ability if you press button to activate it
		you might want to display AoE targetting circle first, and after second input press, activate
		ability to launch it place where targetting circle is.

		If this is false, ability will launch instantly after button press.
	*/
	UPROPERTY(EditAnywhere, Category = "Ability")
		bool bHasPreparationStage;

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
	
	void InputPressed();
	void InputReleased();

	void ActivateAbility();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerActivateAbility();

	void DeactivateAbility();
	UFUNCTION(Server, Reliable, WithValidation)
		void ServerDeactivateAbility();

	/**
	 *	Blueprint Events
	 */
	/**
	 *	Called when ability is Activated (ie. casting finished).
	 *	Make it overridable in blueprint ? Or just use it to start event graph ?
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Game Abilities")
		void OnAbilityActivated();

	UFUNCTION(BlueprintImplementableEvent, BlueprintAuthorityOnly, Category = "Game Abilities")
		void OnAbilityDeactivated();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownStarted();

	UFUNCTION(BlueprintImplementableEvent, Category = "Game Abilities")
		void OnCooldownEnded();
};
