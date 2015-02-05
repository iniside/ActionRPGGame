#pragma once
#include "GAAttributesBase.h"
#include "ARCharacterAttributes.generated.h"


/*

	Execute effmod object linearlly in single iteration every time appropertiate attribute is modified.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ACTIONRPGGAME_API UARCharacterAttributes : public UGAAttributesBase
{
	GENERATED_UCLASS_BODY()
public:
	//this represents current health available to character.
	UPROPERTY(EditAnywhere, Category = "Resources")
		float Health;
	UPROPERTY(VisibleAnywhere, Category = "Resources")
		float MaxHealth; //max value is the absolute maximum value, that is callculate from 
	//all bonuses, it is never changed unless affecting bonuses changes.

	UPROPERTY(EditAnywhere, Category = "Resources")
		float Energy;
	UPROPERTY(VisibleAnywhere, Category = "Resources")
		float MaxEnergy;
	
	UPROPERTY(EditAnywhere, Category = "Resources")
		float Stamina;
	UPROPERTY(VisibleAnywhere, Category = "Resources")
		float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Strenght;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float StrenghtMod; //like in Dnd = (Strenght - 10) /2 - no clamp, can be negative!
	
	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Endurance;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float EnduranceMod;
	
	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Agility;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float AgilityMod;

	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Intelligence;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float IntelligenceMod;

	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Magic;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float MagicMod;

	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float WillPower;
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float WillPowerMod;

	UPROPERTY(EditAnywhere, Category = "Base Attributes")
		float Wisdom; 
	UPROPERTY(VisibleAnywhere, Category = "Base Attributes")
		float WisdomMod;

	/*
		And now we descned into really interesting part..
		That I don't really know how to handle. Yet.

		Assuming we have these damage types:
		Physical
		Magical
		Fire
		Water
		Ice
		Poison
		Necrotic
		Shadow
		Darkness
		Earth
		Electricity
		Light
		(list is to short)

		We need to handle few things, at the class above this one.
		1. We need to determine damage type. That's easy part, because we will know it by tags
		coming along with attribute modifier.
		2. How we determine by tag, which attribute should be considered for modification ?
		If incoming is Damage.Fire, we want to modify FireDamge Attribute (so we can calculate all
		applicable bonuses to fire damage), and then apply it to the target Health
		Health - FireDamage.
	*/



	/*
		Helper attributes, which are used to apply different types of damage. Ahoy!
	*/
	UPROPERTY()
		float Damage;
	UPROPERTY()
		float PhysicalDamage;
	UPROPERTY()
		float MagicalDamage;
	UPROPERTY()
		float FireDamage;

	UPROPERTY()
		float ConditionDamage;
	UPROPERTY()
		float ConditionFireDamage;
	UPROPERTY()
		float ConditionBleedDamage;
	UPROPERTY()
		float ConditionPoisonDamage;

	UPROPERTY()
		float WeaknessCondition; //0-1, precentage. always appilied on Source, reduce damage.

	/*
		Because Why not ?
	*/
	/*
		Modify these attributes, to steal resource (from target), and transfer it to instigator.
		Makes sense uhh ?
	*/
	UPROPERTY()
		float LifeStealDamage;
	UPROPERTY()
		float EnergyStealDamage;
	UPROPERTY()
		float StaminaStealDamage;

	UPROPERTY(EditAnywhere, Category = "Damage")
		float ConditionBonusDamage;
	/*
		Shouldn't be editable, but for testing simplicity..
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusPhysicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusMagicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusFireDamage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float FireDamageDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		float SpellCastingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		float AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		float SpellCostMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		float PhysicalAttackCostMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float RunningMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		float WalkingMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight")
		float CurrentWeight;
	virtual void PostInterpChange(UProperty* PropertyThatChanged) override;
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool PostModifyAttribute_Implementation(const FGAEvalData& AttributeMod) override;
};
