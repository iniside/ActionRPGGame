#pragma once
#include "GAAttributesBase.h"
#include "GameplayTagContainer.h"
#include "ARCharacterAttributes.generated.h"

DECLARE_STATS_GROUP(TEXT("GameAttributes"), STATGROUP_GameAttributes, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostModifyAttribute"), STAT_PostModifyAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostEffectApplied"), STAT_PostEffectApplied, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesOutgoingAttribute"), STAT_OutgoingAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesIncomingAttribute"), STAT_IncomingAttribute, STATGROUP_GameAttributes, );

/*

	Execute effmod object linearlly in single iteration every time appropertiate attribute is modified.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ACTIONRPGGAME_API UARCharacterAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UARCharacterAttributes(const FObjectInitializer& ObjectInitializer);
	//UPROPERTY(EditAnywhere, Category = "Tags Configuration")
	//	FGameplayTagContainer FireDamageTag;

	/*
		This is base value of health, it should never be modified directly (??).

		Idk about calculation like each point of Constution gives 10hp.
		Should it go into base, or into bonus ?
		Or maybe another attribute, which is only used to store value of health
		calculated from other attributes ?
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase Energy;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		FGAAttributeBase Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Strenght;
	/*
		Strenght mod is directly dependand on strenght.
		How should we handle this depedency ? On attribute level, on here on
		attribute object ?
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase StrenghtMod; //like in Dnd = (Strenght - 10) /2 - no clamp, can be negative!
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Endurance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase EnduranceMod;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Constitution;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase ConstitutionMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Agility;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase AgilityMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Intelligence;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase IntelligenceMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Magic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase MagicMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase WillPower;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase WillPowerMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Wisdom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase WisdomMod;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase Armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase MagicResitance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase FireResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase IceResistance;

	UPROPERTY()
		float HealthPercentageDamage;
	UPROPERTY()
		float EnergyPercentageDamage;
	UPROPERTY()
		float StaminaPercentageDamage;

	UPROPERTY()
		float HealthCost;
	UPROPERTY()
		float EnergyCost;//??
	UPROPERTY()
		float EnergyCostReduce;//?? precentage ?? flat number ? can have effect, which set this.
	UPROPERTY()
		float EnergyIncrease;//??

	UPROPERTY()
		float PercentageDamage;

	UPROPERTY()
		float StaminaCost;

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
		float IceDamage;
	UPROPERTY()
		float WaterDamage;
	UPROPERTY()
		float AirDamage;
	UPROPERTY()
		float ElectricityDamage;
	UPROPERTY()
		float EarthDamage;
	UPROPERTY()
		float ShadowDamage;
	UPROPERTY()
		float NecroticDamage;

	UPROPERTY()
		float Heal;

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


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase DamageBonus;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FGAAttributeBase FireDamageBonus;

	///*
	//	Total number of conditions.
	//*/
	UPROPERTY()
		FGAAttributeBase ConditionCount;
	UPROPERTY()
		FGAAttributeBase HexesCount;
	UPROPERTY()
		FGAAttributeBase CursesCount;
	UPROPERTY()
		FGAAttributeBase EnchatmentsCount;

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

		Precentages (0-1).

		Do not stack, BonusDamage override everything (if highest)
		otherwise attribute specific to damage type is used.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase BonusDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase BonusPhysicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase BonusMagicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase BonusFireDamage;
	//because why not ?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase OutgoingDamageReduction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		FGAAttributeBase FireDamageDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FGAAttributeBase SpellCastingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FGAAttributeBase AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FGAAttributeBase SpellCostMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FGAAttributeBase PhysicalAttackCostMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FGAAttributeBase RunningMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FGAAttributeBase WalkingMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight")
		FGAAttributeBase CurrentWeight;
protected:
	TMap<FName, TWeakObjectPtr<UFunction>> PostModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> IncomingModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> OutgoingModifyAttributeFunctions;

public:
	virtual void InitializeAttributes() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		void InternalEffectParams();

	virtual void PostEffectApplied() override;
	virtual void PostEffectRemoved(const FGAEffectHandle& HandleIn, const FGAEffectSpec& SpecIn) override;

	/*
		This need simpler data structure for modification in blueprint.
	*/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		float InternalPostModifyAttribute(const FGAEvalData& AttributeMod);
	virtual float PostModifyAttribute(const FGAEvalData& AttributeMod) override;


	virtual void CalculateOutgoingAttributeMods() override;
	virtual void CalculateIncomingAttributeMods() override;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		void InternalIncomingAttributeMod();

	//UFUNCTION(Category = "PostAttribute")
	//	FGAAttributeDataCallback PostAttribute_Health(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_Damage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_FireDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_Heal(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_LifeStealDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_HealthBakPrecentageReduction(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		float PostAttribute_Magic(const FGAEvalData& AttributeMod);

};
