#pragma once
#include "Attributes/GAAttributesBase.h"
#include "GameplayTagContainer.h"
#include "ARCharacterAttributes.generated.h"

DECLARE_STATS_GROUP(TEXT("GameAttributes"), STATGROUP_GameAttributes, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostModifyAttribute"), STAT_PostModifyAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostEffectApplied"), STAT_PostEffectApplied, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesOutgoingAttribute"), STAT_OutgoingAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesIncomingAttribute"), STAT_IncomingAttribute, STATGROUP_GameAttributes, );

/*
	The basic workflow idea:

	1. Create scalar(float) attributes, which are transient. They should never be saved and
	always initialized to 0. For example, attributes like Damage, FireDamage, Heal, EnergyCost,
	PercentageDamage.
	For most of games, these will be attributes, which simply modify other non-transient attributes.
	
	2. Use FAFAttributeBase or derived from, to create normal attributes, like Health,
	Energy, DamageBonus, FireDamageBonus etc. 
	These attributes can be safely serialized, and are able to track their own state
	(what are my modifiers, who modify me etc).

	3. When scalar attribute is modified, it will go trough two functions:
	PreModifyAttribute, PostModifyAttribute.
	You should implement functions named PostAttribute_AttributeName
	and PreAttribute_AttributeName

	PreModifyAttribute is called, before attribtue is being applied to target, and it
	is called on both instigator and target.

	PostModifyAttribute is called only on target, and in general you should use it to handle, 
	basic interaction between attributes, not to calculate attribute values.

	You probabaly will want to split PreAttribute functions into two parts. Outgoing and Incoming.
	You don't want Instigator armor, to reduce Outgoing damage, applied to target.
	But you probabaly want, target armor to reduce incoming damage.
	There might be a better way to handle it.

	The upside of this approach is the fact, that we have very fine controll, over
	what exactly affect attribute, and how it will stack (DamageBonus, for 
	example might not affect FireDamage).

	The downside is that, we have to maintain long list of functions, which
	must fallow very specific pattern of naming.
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
		How often timer will tick to add attribute.
	*/
	UPROPERTY(EditAnywhere, Category = "Config")
		float AttributeRefreshRate;


	/*
		This is base value of health, it should never be modified directly (??).

		Idk about calculation like each point of Constution gives 10hp.
		Should it go into base, or into bonus ?
		Or maybe another attribute, which is only used to store value of health
		calculated from other attributes ?
	*/
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase Health;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase Energy;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Resources")
		FAFAttributeBase Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Strenght;
	/*
		Strenght mod is directly dependand on strenght.
		How should we handle this depedency ? On attribute level, on here on
		attribute object ?
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase StrenghtMod; //like in Dnd = (Strenght - 10) /2 - no clamp, can be negative!
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Endurance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase EnduranceMod;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Constitution;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase ConstitutionMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Agility;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase AgilityMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Intelligence;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase IntelligenceMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Magic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase WillPower;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase WillPowerMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Wisdom;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase WisdomMod;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase Armor;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase MagicResitance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase FireResistance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		FAFAttributeBase IceResistance;

	/*
		Helper attributes, which are used to apply different types of damage. Ahoy!
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		FAFAttributeBase MagicalDamage;

	/*
	Modify these attributes, to steal resource (from target), and transfer it to instigator.
	Makes sense uhh ?
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		float LifeStealDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		float EnergyStealDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Damage")
		float StaminaStealDamage;

	UPROPERTY()
		float WeaknessCondition; //0-1, precentage. always appilied on Source, reduce damage.

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Headling")
		float Heal;

	///*
	//	Total number of conditions.
	//*/
	UPROPERTY()
		FAFAttributeBase ConditionCount;
	UPROPERTY()
		FAFAttributeBase HexesCount;
	UPROPERTY()
		FAFAttributeBase CursesCount;
	UPROPERTY()
		FAFAttributeBase EnchatmentsCount;

	/*
		Because Why not ?
	*/


	/*
		Shouldn't be editable, but for testing simplicity..

		Precentages (0-1).

		Do not stack, BonusDamage override everything (if highest)
		otherwise attribute specific to damage type is used.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase BonusDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase BonusPhysicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase BonusMagicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase BonusFireDamage;
	//because why not ?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase OutgoingDamageReduction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Bonus")
		FAFAttributeBase WeaponDamageBonus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Defense")
		FAFAttributeBase FireDamageDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage Defense")
		FAFAttributeBase DamageDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FAFAttributeBase SpellCastingSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FAFAttributeBase AttackSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FAFAttributeBase SpellCostMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
		FAFAttributeBase PhysicalAttackCostMultiplier;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FAFAttributeBase RunningMovementSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement")
		FAFAttributeBase WalkingMovementSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weight")
		FAFAttributeBase CurrentWeight;
protected:
	TMap<FName, TWeakObjectPtr<UFunction>> PostModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> IncomingModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> OutgoingModifyAttributeFunctions;

public:
	virtual void InitializeAttributes(UGAAbilitiesComponent* InOwningAttributeComp) override;
};
