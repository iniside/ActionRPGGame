#pragma once
#include "GAAttributesBase.h"
#include "GameplayTagContainer.h"
#include "ARCharacterAttributes.generated.h"

DECLARE_STATS_GROUP(TEXT("GameAttributes"), STATGROUP_GameAttributes, STATCAT_Advanced);
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostModifyAttribute"), STAT_PostModifyAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesPostEffectApplied"), STAT_PostEffectApplied, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesOutgoingAttribute"), STAT_OutgoingAttribute, STATGROUP_GameAttributes, );
DECLARE_CYCLE_STAT_EXTERN(TEXT("GameAttributesIncomingAttribute"), STAT_IncomingAttribute, STATGROUP_GameAttributes, );

USTRUCT()
struct FGAAttributeNode
{
	GENERATED_USTRUCT_BODY()
public:
	int32 AttributeIndex;
	FName AttributeName;

	UPROPERTY()
		TWeakObjectPtr<UFunction> AttributeFunction;
};
/*

	Execute effmod object linearlly in single iteration every time appropertiate attribute is modified.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInlineNew)
class ACTIONRPGGAME_API UARCharacterAttributes : public UGAAttributesBase
{
	GENERATED_BODY()
public:
	UARCharacterAttributes(const FObjectInitializer& ObjectInitializer);
	UPROPERTY()
		TArray<FGAAttributeNode> AttributeNodes;
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

	UPROPERTY()
		float BaseHealth;
	/*
		Health = Clamp(BaseHealth + BonusHealth, MaxHealth)
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		float HealthBak;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resources")
		float MaxHealthBak; //max value is the absolute maximum value, that is callculate from 
	//all bonuses, it is never changed unless affecting bonuses changes.
	/*
		Current bonus health, from various effects.
	*/
	UPROPERTY()
		float BonusHealth;
		
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		float Energy;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resources")
		float MaxEnergy;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resources")
		float Stamina;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Resources")
		float MaxStamina;

	UPROPERTY()
		float HealthCost;
	UPROPERTY()
		float EnergyCost;//??
	UPROPERTY()
		float EnergyCostReduce;//?? precentage ?? flat number ? can have effect, which set this.
	UPROPERTY()
		float EnergyIncrease;//??

	UPROPERTY()
		float PrecentageDamage;
	UPROPERTY()
		float HealthBakPrecentageReduction;

	UPROPERTY()
		float StaminaCost;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Strenght;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float StrenghtMod; //like in Dnd = (Strenght - 10) /2 - no clamp, can be negative!
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Endurance;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float EnduranceMod;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Agility;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float AgilityMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Intelligence;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float IntelligenceMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Magic;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float MagicMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float WillPower;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float WillPowerMod;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float Wisdom; 
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Base Attributes")
		float WisdomMod;
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
		Total number of conditions.
	*/
	UPROPERTY()
		float ConditionCount;
	UPROPERTY()
		float HexesCount;
	UPROPERTY()
		float CursesCount;

	UPROPERTY()
		float EnchatmentsCount;

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
		float BonusDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusPhysicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusMagicalDamage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float BonusFireDamage;
	//because why not ?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float OutgoingDamageReduction;

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
protected:
	TMap<FName, TWeakObjectPtr<UFunction>> PostModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> IncomingModifyAttributeFunctions;
	TMap<FName, TWeakObjectPtr<UFunction>> OutgoingModifyAttributeFunctions;

public:
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void InitializeAttributes() override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		FGAAttributeDataCallback InternalEffectParams(FGAEffectBase& SpecIn);

	virtual void PostEffectApplied(FGAEffectBase& SpecIn) override;
	virtual void PostEffectRemoved(FGAEffectBase& SpecIn) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		FGAAttributeDataCallback InternalPostModifyAttribute(const FGAEvalData& AttributeMod);
	virtual FGAAttributeDataCallback PostModifyAttribute(const FGAEvalData& AttributeMod) override;


	virtual FGAAttributeSpec CalculateOutgoingAttributeMods(const FGAAttributeSpec& AttributeModIn) override;
	virtual FGAAttributeSpec CalculateIncomingAttributeMods(const FGAAttributeSpec& AttributeModIn) override;


	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = AttributeTags, meta = (BlueprintInternalUseOnly = "true"))
		FGAAttributeSpec InternalIncomingAttributeMod(const FGAAttributeSpec& AttributeModIn);

	UFUNCTION(Category = "Incoming")
		FGAAttributeSpec Incoming_Damage_Fire(const FGAAttributeSpec& AttributeModIn);
	UFUNCTION(Category = "Outgoing")
		FGAAttributeSpec Outgoing_Damage_Fire(const FGAAttributeSpec& AttributeModIn);

	//UFUNCTION(Category = "PostAttribute")
	//	FGAAttributeDataCallback PostAttribute_Health(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Damage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_LifeStealDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_HealthBakPrecentageReduction(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Magic(const FGAEvalData& AttributeMod);

	//ignore it. I'm just profiling performance of PostModifyAttributeFunctions map
	//with lots of entries.
	//this technically could be just as well dont using switch/if/else if neede but.
	//using map, allow for far easier extending in blueprints... as long as your functions
	//have right name.
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Strenght(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Endurance(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Intelligence(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_WillPower(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Wisdom(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_ConditionDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_ConditionFireDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_ConditionBleedDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_ConditionPoisonDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_BonusDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_BonusPhysicalDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_BonusMagicalDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_FireDamageDefense(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_SpellCastingSpeed(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_AttackSpeed(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_SpellCostMultiplier(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_PhysicalAttackCostMultiplier(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_EnergyStealDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_StaminaStealDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_RunningMovementSpeed(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_WalkingMovementSpeed(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_PhysicalDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_MagicalDamage(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_FireDamage(const FGAEvalData& AttributeMod);

	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Hex(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Somg(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Ehehehhee(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_AnotherAttribute(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Zomg(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Dunno(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Moar(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Roflolamo(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_EvenMoreFunction(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_Notsogood(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_postpooasd(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_uahskjd(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_kjashkd(const FGAEvalData& AttributeMod);
	UFUNCTION(Category = "PostAttribute")
		FGAAttributeDataCallback PostAttribute_uayskahsd(const FGAEvalData& AttributeMod);

	UFUNCTION(Category = "PostEffectApplied")
		void OnEffectApplied_Hex(const FGAEffectBase& AttributeMod);
	UFUNCTION(Category = "PostEffectRemoved")
		void OnEffectRemoved_Hex(const FGAEffectBase& AttributeMod);

	UFUNCTION(Category = "PostEffectApplied")
		void OnEffectApplied_Condition_Weakness(const FGAEffectBase& AttributeMod);
	UFUNCTION(Category = "PostEffectRemoved")
		void OnEffectRemoved_Condition_Weakness(const FGAEffectBase& AttributeMod);

};
