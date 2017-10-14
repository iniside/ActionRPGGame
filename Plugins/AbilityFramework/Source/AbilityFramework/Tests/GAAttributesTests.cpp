// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../AbilityFramework.h"
#include "AutomationTest.h"
#include "GameplayTagsModule.h"
#include "../GAGlobalTypes.h"
#include "../Attributes/GAAttributeBase.h"
#include "../Effects/GAGameEffect.h"
#include "../AFAbilityComponent.h"
#include "../Attributes/GAAttributesBase.h"
#include "../Effects/GAEffectExecution.h"
#include "../Effects/GABlueprintLibrary.h"
#include "GAAttributesTest.h"
#include "GASpellExecutionTest.h"
#include "GACharacterAttributeTest.h"
#include "GACustomCalculationTest.h"
#include "GAffectSpecTestOne.h"
#include "../Effects/ApplicationRequirement/AFAttributeStongerOverride.h"
#include "../Effects/CustomApplications/AFAttributeDurationOverride.h"
#include "../Effects/CustomApplications/AFPeriodApplicationOverride.h"
#include "../Effects/CustomApplications/AFAtributeDurationAdd.h"
#include "../Effects/CustomApplications/AFPeriodApplicationAdd.h"
#include "../Effects/CustomApplications/AFAttributeDurationInfinite.h"
#include "../Effects/CustomApplications/AFPeriodApplicationExtend.h"

#if WITH_EDITOR

static UDataTable* CreateGameplayDataTable()
{
	FString CSV(TEXT(",Tag,CategoryText,"));
	CSV.Append(TEXT("\r\n0,Damage"));
	CSV.Append(TEXT("\r\n1,Damage.Basic"));
	CSV.Append(TEXT("\r\n2,Damage.Type1"));
	CSV.Append(TEXT("\r\n3,Damage.Type2"));
	CSV.Append(TEXT("\r\n4,Damage.Reduce"));
	CSV.Append(TEXT("\r\n5,Damage.Buffable"));
	CSV.Append(TEXT("\r\n6,Damage.Buff"));
	CSV.Append(TEXT("\r\n7,Damage.Physical"));
	CSV.Append(TEXT("\r\n8,Damage.Fire"));
	CSV.Append(TEXT("\r\n9,Damage.Buffed.FireBuff"));
	CSV.Append(TEXT("\r\n10,Damage.Mitigated.Armor"));
	CSV.Append(TEXT("\r\n11,Lifesteal"));
	CSV.Append(TEXT("\r\n12,Shield"));
	CSV.Append(TEXT("\r\n13,Buff"));
	CSV.Append(TEXT("\r\n14,Immune"));
	CSV.Append(TEXT("\r\n15,FireDamage"));
	CSV.Append(TEXT("\r\n16,ShieldAbsorb"));
	CSV.Append(TEXT("\r\n17,Stackable"));
	CSV.Append(TEXT("\r\n18,Stack"));
	CSV.Append(TEXT("\r\n19,Stack.CappedNumber"));
	CSV.Append(TEXT("\r\n20,Stack.DiminishingReturns"));
	CSV.Append(TEXT("\r\n21,Protect.Damage"));
	CSV.Append(TEXT("\r\n22,SpellDmg.Buff"));
	CSV.Append(TEXT("\r\n23,GameplayCue.Burning"));
	CSV.Append(TEXT("\r\n24,Damage.Fire"));
	CSV.Append(TEXT("\r\n25,Damage.Ice"));
	CSV.Append(TEXT("\r\n26,Damage.Acid"));
	CSV.Append(TEXT("\r\n27,Damage.Earth"));
	CSV.Append(TEXT("\r\n28,Damage.Electricity"));
	CSV.Append(TEXT("\r\n29,Damage.Darkness"));
	CSV.Append(TEXT("\r\n30,Damage.Necrotic"));
	CSV.Append(TEXT("\r\n31,Damage.Radiant"));
	CSV.Append(TEXT("\r\n32,Damage.Water"));

	auto DataTable = NewObject<UDataTable>(GetTransientPackage(), FName(TEXT("TempDataTable")));
	DataTable->RowStruct = FGameplayTagTableRow::StaticStruct();
	DataTable->CreateTableFromCSVString(CSV);

	FGameplayTagTableRow * Row = (FGameplayTagTableRow*)DataTable->RowMap["0"];
	if (Row)
	{
		check(Row->Tag == TEXT("Damage"));
	}
	return DataTable;
};

struct FTagsInput
{
	FGameplayTagContainer OwnedTags;
	FGameplayTagContainer EffectTags;
	FGameplayTagContainer AttributeTags;
	FGameplayTagContainer DenyTags;
	FGameplayTagContainer AppliedImmunityTags;
	FGameplayTagContainer ApplyTags;
	FGameplayTagContainer RequiredTags;
	FGameplayTagContainer OngoingRequiredTags;
};
#define CONSTRUCT_CLASS(Class, Name) Class* Name = NewObject<Class>(GetTransientPackage(), FName(TEXT(#Name)))
class GameEffectsTestSuite
{
	UWorld* World;
	FAutomationTestBase* Test;

	AGACharacterAttributeTest* SourceActor;
	UAFAbilityComponent* SourceComponent;

	AGACharacterAttributeTest* DestActor;
	UAFAbilityComponent* DestComponent;

	AGACharacterAttributeTest* TargetOne;
	UAFAbilityComponent* TargetCompOne;

	AGACharacterAttributeTest* TargetTwo;
	UAFAbilityComponent* TargetCompTwo;

public:
	GameEffectsTestSuite(UWorld* WorldIn, FAutomationTestBase* TestIn)
		: World(WorldIn),
		Test(TestIn)
	{
		SourceActor = World->SpawnActor<AGACharacterAttributeTest>();
		SourceComponent = SourceActor->Attributes;
		SourceComponent->DefaultAttributes = NewObject<UGAAttributesTest>(SourceActor->Attributes);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.SetMaxValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.SetMaxValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetMaxValue(500);
		/*SourceComponent->GetAttributes<UGAAttributesTest>()->Health.Stacking = EAFAttributeStacking::StrongerOverride;
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.Stacking = EAFAttributeStacking::Override;
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.Stacking = EAFAttributeStacking::Add;
*/
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetMaxValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetMaxValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.SetMaxValue(500);

		//SourceComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.Stacking = EAFAttributeStacking::Override;
		//SourceComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.Stacking = EAFAttributeStacking::Add;
		//SourceComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.Stacking = EAFAttributeStacking::Override;

		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute(SourceComponent, TEXT("Health"));
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute(SourceComponent, TEXT("Energy"));
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute(SourceComponent, TEXT("Stamina"));

		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute(SourceComponent, TEXT("MagicalBonus"));
		SourceComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute(SourceComponent, TEXT("PhysicalBonus"));
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute(SourceComponent, TEXT("MagicResistance"));
		SourceComponent->DefaultAttributes->InitializeAttributes(SourceComponent);

		DestActor = World->SpawnActor<AGACharacterAttributeTest>();
		DestComponent = DestActor->Attributes;
		DestComponent->DefaultAttributes = NewObject<UGAAttributesTest>(DestActor->Attributes);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetMaxValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetMaxValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetMaxValue(500);
		/*DestComponent->GetAttributes<UGAAttributesTest>()->Health.Stacking = EAFAttributeStacking::StrongerOverride;
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.Stacking = EAFAttributeStacking::Override;
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.Stacking = EAFAttributeStacking::Add;*/
		DestComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetMaxValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetMaxValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.SetMaxValue(500);

		DestComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute(DestComponent, TEXT("Health"));
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute(DestComponent, TEXT("Energy"));
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute(DestComponent, TEXT("Stamina"));

		DestComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute(DestComponent, TEXT("MagicalBonus"));
		DestComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute(DestComponent, TEXT("PhysicalBonus"));
		DestComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute(DestComponent, TEXT("MagicResistance"));
		DestComponent->DefaultAttributes->InitializeAttributes(DestComponent);

		TargetOne = World->SpawnActor<AGACharacterAttributeTest>();
		TargetCompOne = TargetOne->Attributes;
		TargetCompOne->DefaultAttributes = NewObject<UGAAttributesTest>(TargetOne->Attributes);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Health.SetMaxValue(500);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Energy.SetMaxValue(500);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Stamina.SetMaxValue(500);
		//TargetCompOne->GetAttributes<UGAAttributesTest>()->Health.Stacking = EAFAttributeStacking::StrongerOverride;
		//TargetCompOne->GetAttributes<UGAAttributesTest>()->Energy.Stacking = EAFAttributeStacking::Override;
		//TargetCompOne->GetAttributes<UGAAttributesTest>()->Stamina.Stacking = EAFAttributeStacking::Add;
		TargetCompOne->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetMaxValue(500);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetMaxValue(500);
		TargetCompOne->GetAttributes<UGAAttributesTest>()->MagicResistance.SetMaxValue(500);

		TargetCompOne->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute(TargetCompOne, TEXT("Health"));
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute(TargetCompOne, TEXT("Energy"));
		TargetCompOne->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute(TargetCompOne, TEXT("Stamina"));

		TargetCompOne->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute(TargetCompOne, TEXT("MagicalBonus"));
		TargetCompOne->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute(TargetCompOne, TEXT("PhysicalBonus"));
		TargetCompOne->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute(TargetCompOne, TEXT("MagicResistance"));
		TargetCompOne->DefaultAttributes->InitializeAttributes(TargetCompOne);


		TargetTwo = World->SpawnActor<AGACharacterAttributeTest>();
		TargetCompTwo = TargetTwo->Attributes;
		TargetCompTwo->DefaultAttributes = NewObject<UGAAttributesTest>(TargetTwo->Attributes);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Health.SetMaxValue(500);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Energy.SetMaxValue(500);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Stamina.SetMaxValue(500);
		//TargetCompTwo->GetAttributes<UGAAttributesTest>()->Health.Stacking = EAFAttributeStacking::StrongerOverride;
		//TargetCompTwo->GetAttributes<UGAAttributesTest>()->Energy.Stacking = EAFAttributeStacking::Override;
		//TargetCompTwo->GetAttributes<UGAAttributesTest>()->Stamina.Stacking = EAFAttributeStacking::Add;
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetMaxValue(500);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetMaxValue(500);
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->MagicResistance.SetMaxValue(500);

		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute(TargetCompTwo, TEXT("Health"));
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute(TargetCompTwo, TEXT("Energy"));
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute(TargetCompTwo, TEXT("Stamina"));

		TargetCompTwo->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute(TargetCompTwo, TEXT("MagicalBonus"));
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute(TargetCompTwo, TEXT("PhysicalBonus"));
		TargetCompTwo->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute(TargetCompTwo, TEXT("MagicResistance"));
		TargetCompTwo->DefaultAttributes->InitializeAttributes(TargetCompTwo);

		//DestActor->BeginPlay();
		//SourceActor->BeginPlay();
	}

	~GameEffectsTestSuite()
	{
		// run after each test

		// destroy the actors
		if (SourceActor)
		{
			World->EditorDestroyActor(SourceActor, false);
		}
		if (DestActor)
		{
			World->EditorDestroyActor(DestActor, false);
		}
	}
	void TickWorld(float Time)
	{
		const float step = 0.01f;
		while (Time > 0.f)
		{
			World->Tick(ELevelTick::LEVELTICK_All, FMath::Min(Time, step));
			Time -= step;

			// This is terrible but required for subticking like this.
			// we could always cache the real GFrameCounter at the start of our tests and restore it when finished.
			GFrameCounter++;
		}
	}
	FGameplayTag RequestTag(const FName& TagIn)
	{
		return UGameplayTagsManager::Get().RequestGameplayTag(TagIn);
	}

	FGameplayTagContainer CreateTags(const TArray<FName>& TagsIn)
	{
		FGameplayTagContainer OutTags;
		TArray<FGameplayTag> Tags;
		for (const FName& Tag : TagsIn)
		{
			FGameplayTag ReqTeg = UGameplayTagsManager::Get().RequestGameplayTag(Tag);
			if (ReqTeg.IsValid())
			{
				Tags.Add(ReqTeg);
			}
		}
		OutTags = FGameplayTagContainer::CreateFromArray(Tags);
		//OutTags.FillParentTags();
		return OutTags;
	}

	void TestEqual(const FString& TestText, float Actual, float Expected)
	{
		Test->TestEqual(FString::Printf(TEXT("%s: %f (actual) != %f (expected)"), *TestText, Actual, Expected), Actual, Expected);
	}
	void TestEqual(const FString& TestText, int32 Actual, int32 Expected)
	{
		Test->TestEqual(FString::Printf(TEXT("%s: %f (actual) != %f (expected)"), *TestText, Actual, Expected), Actual, Expected);
	}
	void TestEqualTags(const FString& TestText, FName Actual, FName Expected)
	{
		Test->TestEqual(FString::Printf(TEXT("%s: %s (actual) != %s (expected)"), *TestText, *Actual.ToString(), *Expected.ToString()), Actual, Expected);
	}
	void TestEqualTags(const FString& TestText, FGameplayTag Actual, FGameplayTag Expected)
	{
		Test->TestEqual(FString::Printf(TEXT("%s: %s (actual) != %s (expected)"), *TestText, *Actual.ToString(), *Expected.ToString()), Actual, Expected);
	}
	FGAAttributeModifier CreateAttributeModifer(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute)
	{
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;

		return AttributeModifier;
	}

	FGAEffectProperty CreateEffectSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, TSubclassOf<UGAGameEffectSpec> CDO)
	{
		TSubclassOf<UGAGameEffectSpec> c = CDO;
		FGAEffectProperty Spec;
		//UGAGameEffectSpec* Effect = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec = c;
		//Spec = Effect;
		UGAGameEffectSpec* cds = Spec.GetClass().GetDefaultObject();
		cds->ExecutionType = UGASpellExecutionTest::StaticClass();
		cds->ApplicationRequirement = UAFEffectApplicationRequirement::StaticClass();
		cds->Application = UAFEffectCustomApplication::StaticClass();
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		cds->OwnedTags = CreateTags(OwnedTags);
		cds->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = 0;
		cds->Duration = DurationMag;
		FGAMagnitude PeriodMag;
		PeriodMag.CalculationType = EGAMagnitudeCalculation::Direct;
		PeriodMag.DirectModifier.Value = 0;
		cds->Period = PeriodMag;
		return Spec;
	}
	FGAEffectProperty CreateEffectDurationSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, EGAEffectStacking Stacking,
		const TArray<FName>& AttributeTags = TArray<FName>(),
		const TArray<FName>& ApplyTags = TArray<FName>(),
		const FTagsInput& TagsIn = FTagsInput(),
		TSubclassOf<UAFEffectApplicationRequirement> ApplReq = UAFEffectApplicationRequirement::StaticClass(),
		TSubclassOf<UAFEffectCustomApplication> Appl = UAFEffectCustomApplication::StaticClass(),
		TSubclassOf<UGAGameEffectSpec> CDO = UGAGameEffectSpec::StaticClass()
	)
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectProperty Spec;
		Spec = CDO;
		UGAGameEffectSpec* cdo = Spec.GetClass().GetDefaultObject();
		
		//UClass* clas = DuplicateObject<UClass>(UGAGameEffectSpec::StaticClass());
		//Spec.Spec = 
		cdo->ExecutionType = UGAEffectExecution::StaticClass();
		cdo->ApplicationRequirement = ApplReq;
		cdo->Application = Appl;
		cdo->OwnedTags = CreateTags(OwnedTags);
		cdo->bExecuteOnApplication = true;
		if (AttributeTags.Num() > 0)
			cdo->AttributeTags = CreateTags(AttributeTags);
		if (ApplyTags.Num() > 0)
			cdo->ApplyTags = CreateTags(ApplyTags);

		cdo->RequiredTags = TagsIn.RequiredTags;
		cdo->DenyTags = TagsIn.DenyTags;
		cdo->ExecutionRequiredTags = TagsIn.OngoingRequiredTags;

		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		cdo->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = Duration;
		cdo->Duration = DurationMag;
		FGAMagnitude PeriodMag;
		PeriodMag.CalculationType = EGAMagnitudeCalculation::Direct;
		PeriodMag.DirectModifier.Value = 0;
		cdo->Period = PeriodMag;
		return Spec;
	}

	FGAEffectProperty CreateEffectPeriodicSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, EGAEffectStacking Stacking,
		const TArray<FName>& AttributeTags = TArray<FName>(),
		const TArray<FName>& ApplyTags = TArray<FName>(),
		const FTagsInput& TagsIn = FTagsInput(),
		TSubclassOf<UGAGameEffectSpec> CDO = UGAGameEffectSpec::StaticClass(),
		TSubclassOf<UAFEffectCustomApplication> Appl = UAFEffectCustomApplication::StaticClass(),
		bool InExecuteOnApplication = false
	)
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectProperty Spec;
		Spec = CDO;
		UGAGameEffectSpec* cdo = Spec.GetClass().GetDefaultObject();
		
		cdo->ExecutionType = UGAEffectExecution::StaticClass();
		cdo->ApplicationRequirement = UAFAttributeStongerOverride::StaticClass();
		cdo->Application = Appl;

		cdo->OwnedTags = CreateTags(OwnedTags);
		if (AttributeTags.Num() > 0)
			cdo->AttributeTags = CreateTags(AttributeTags);
		if (ApplyTags.Num() > 0)
			cdo->ApplyTags = CreateTags(ApplyTags);

		cdo->RequiredTags = TagsIn.RequiredTags;
		cdo->DenyTags = TagsIn.DenyTags;
		cdo->ExecutionRequiredTags = TagsIn.OngoingRequiredTags;
		cdo->bExecuteOnApplication = InExecuteOnApplication;
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		cdo->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = Duration;
		cdo->Duration = DurationMag;
		FGAMagnitude PeriodMag;
		PeriodMag.CalculationType = EGAMagnitudeCalculation::Direct;
		PeriodMag.DirectModifier.Value = PeriodSecs;
		cdo->Period = PeriodMag;
		return Spec;
	}

	FGAEffectProperty CreateEffectInfiniteSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, EGAEffectStacking Stacking,
		const TArray<FName>& AttributeTags = TArray<FName>(),
		const TArray<FName>& ApplyTags = TArray<FName>(),
		const FTagsInput& TagsIn = FTagsInput(),
		TSubclassOf<UGAGameEffectSpec> CDO = UGAGameEffectSpec::StaticClass(),
		TSubclassOf<UAFEffectApplicationRequirement> ApplReq = UAFEffectApplicationRequirement::StaticClass(),
		TSubclassOf<UAFEffectCustomApplication> Appl = UAFEffectCustomApplication::StaticClass()
	)
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectProperty Spec;
		Spec = CDO;
		UGAGameEffectSpec* cdo = Spec.GetClass().GetDefaultObject();

		//UClass* clas = DuplicateObject<UClass>(UGAGameEffectSpec::StaticClass());
		//Spec.Spec = 
		cdo->ApplicationRequirement = ApplReq;
		cdo->Application = Appl;
		cdo->ExecutionType = UGAEffectExecution::StaticClass();
		cdo->OwnedTags = CreateTags(OwnedTags);
		cdo->bExecuteOnApplication = true;
		if (AttributeTags.Num() > 0)
			cdo->AttributeTags = CreateTags(AttributeTags);
		if (ApplyTags.Num() > 0)
			cdo->ApplyTags = CreateTags(ApplyTags);

		cdo->RequiredTags = TagsIn.RequiredTags;
		cdo->DenyTags = TagsIn.DenyTags;
		cdo->ExecutionRequiredTags = TagsIn.OngoingRequiredTags;

		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		cdo->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = Duration;
		cdo->Duration = DurationMag;

		return Spec;
	}

	void ApplyEffectModifiers()
	{

	}
	void Test_CompareTagContainers()
	{
		TArray<FName> OwnedTags1;
		OwnedTags1.Add(TEXT("Damage.Ice"));
		OwnedTags1.Add(TEXT("Damage.Spell"));
		FGameplayTagContainer A = CreateTags(OwnedTags1);

		TArray<FName> OwnedTags2;
		OwnedTags2.Add(TEXT("Damage"));
		FGameplayTagContainer B = CreateTags(OwnedTags2);

		TArray<FName> OwnedTags3;
		OwnedTags3.Add(TEXT("Damage.Spell"));
		FGameplayTagContainer C = CreateTags(OwnedTags3);

		TArray<FName> OwnedTags4;
		OwnedTags4.Add(TEXT("Damage.Ice"));
		OwnedTags4.Add(TEXT("Damage.Spell"));
		OwnedTags4.Add(TEXT("Ability.Fireball"));

		FGameplayTagContainer Ability = CreateTags(OwnedTags4);

		bool A_Ability = A.HasAll(Ability); //false
		bool B_Ability = B.HasAll(Ability); //false
		bool C_Ability = C.HasAll(Ability); //false

		Test->TestFalse("A_ABility: ", A_Ability);
		Test->TestFalse("B_ABility: ", B_Ability);
		Test->TestFalse("C_ABility: ", C_Ability);

		bool Ability_A = Ability.HasAll(A); //true
		bool Ability_B = Ability.HasAll(B); //true
		bool Ability_C = Ability.HasAll(C); //true
		Test->TestTrue("Ability_A: ", Ability_A);
		Test->TestTrue("Ability_B: ", Ability_B);
		Test->TestTrue("Ability_C: ", Ability_C);
	}

	void Test_InstantEffectApplication()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		FGAEffectProperty Effect = CreateEffectSpec(OwnedTags, 10,
			EGAAttributeMod::Subtract, "Health", UGAGameEffectSpec::StaticClass());

		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Post: ", PostVal, 90.0f);
	}
	//Damage.Fire
	//Damage.Basic
	//Damage.Buffed.FireBuff
	//Damage.Acid
	//Buff
	//Immune
	//Item
	//Item.Rune
	//Spell
	//Stackable
	//Attribute.Health
	//Condition.Burning
	void Test_PeriodicDamageEffect()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));
		AttributeTags.Add(TEXT("Buff"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Buff"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 5,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationOverride::StaticClass());


		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);
		
		TickWorld(SMALL_NUMBER);
		++NumApplications;
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Post: ", PostVal, 100.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);

			++NumApplications;

			// check that health has been reduced
			
		}
		TickWorld(PeriodSecs);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 55.0f);
	}

	void Test_DurationBuffEffect()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 5,
			EGAAttributeMod::Add, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass());


		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);

			++NumApplications;

			// check that health has been reduced

		}
		TickWorld(PeriodSecs);
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Post: ", PostVal, 55.0f);
	}

	void Test_AtttributeStatckingOverride()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Energy"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
			UAFAttributeDurationOverride::StaticClass(),
			UGAGameEffectSpec::StaticClass()
		);

		

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Energy"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Energy"));
		TestEqual("Source Health PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				FGAEffectProperty EffectWeaker = CreateEffectDurationSpec(OwnedTags, 30,
					EGAAttributeMod::Add, TEXT("Energy"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
					UAFAttributeDurationOverride::StaticClass(),
					UGAGameEffectSpec::StaticClass()
				);
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Energy"));
				TestEqual("Source Health Post2: ", PostVal2, 130.0f);
			}

			// check that health has been reduced

		}
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Energy"));
		TestEqual("Source Health Finished: ", FinishedVal, 100.0f);
	}

	void Test_AtttributeStatckingStrongerOverride()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFAttributeStongerOverride::StaticClass(),
			UAFAttributeDurationOverride::StaticClass(),
			UGAGameEffectSpec::StaticClass()
		);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				FGAEffectProperty EffectWeaker = CreateEffectDurationSpec(OwnedTags, 30,
					EGAAttributeMod::Add, TEXT("Health"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UAFAttributeStongerOverride::StaticClass(),
					UAFAttributeDurationOverride::StaticClass(),
					UGAGameEffectSpec::StaticClass()
				);
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		//for (int32 i = 0; i < NumPeriods; ++i)
		//{
		//	// advance time by one period
		//	TickWorld(PeriodSecs);
		//}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 100.0f);
	}

	void Test_AtttributeStatckingStrongerOverrideReversed()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 30,
			EGAAttributeMod::Add, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFAttributeStongerOverride::StaticClass(),
			UAFAttributeDurationOverride::StaticClass(),
			UGAGameEffectSpec::StaticClass()
		);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 130.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				FGAEffectProperty EffectWeaker = CreateEffectDurationSpec(OwnedTags, 50,
					EGAAttributeMod::Add, TEXT("Health"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UAFAttributeStongerOverride::StaticClass(),
					UAFAttributeDurationOverride::StaticClass(),
					UGAGameEffectSpec::StaticClass()
				);
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 100.0f);
	}

	void Test_AtttributeStatckingeAdd()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
			UAFAtributeDurationAdd::StaticClass(),
			UGAGameEffectSpec::StaticClass()
			);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				FGAEffectProperty EffectWeaker = CreateEffectDurationSpec(OwnedTags, 30,
					EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
					UAFAtributeDurationAdd::StaticClass(),
					UGAGameEffectSpec::StaticClass()
				);
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
				TestEqual("Source Stamina Post2: ", PostVal2, 180.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PreFinished: ", PreFinishedVal, 130.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Finished: ", FinishedVal, 100.0f);
	}
	void Test_AtttributeStatckingeAddSameProp()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
			UAFAtributeDurationAdd::StaticClass(),
			UGAGameEffectSpec::StaticClass()
		);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
				TestEqual("Source Stamina Post2: ", PostVal2, 200.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PreFinished: ", PreFinishedVal, 150.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Finished: ", FinishedVal, 100.0f);
	}

	void Test_AtttributeStatckingeAddSamePropDifferentTargets()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectDurationSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UAFEffectApplicationRequirement::StaticClass(),
			UAFAtributeDurationAdd::StaticClass(),
			UGAGameEffectSpec::StaticClass()
		);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				UGABlueprintLibrary::ApplyGameEffectToActor(Effect, TargetOne, SourceActor, SourceActor, FuncMod);
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
				TestEqual("Source Stamina Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = TargetCompOne->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("TargetCompOne Stamina PreFinished: ", PreFinishedVal, 150.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Finished: ", FinishedVal, 100.0f);

		float TargetOneFinish = TargetCompOne->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("TargetCompOne Stamina PreFinished: ", TargetOneFinish, 100.0f);
	}

	void Test_AtttributeStatckingeAddInfinite()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectInfiniteSpec(OwnedTags, 50,
			EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFEffectApplicationRequirement::StaticClass(),
			UAFAttributeDurationInfinite::StaticClass()
		);

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PPost: ", PostVal, 150.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			//if (i == 5)
			//{
			//	FGAEffectProperty EffectWeaker = CreateEffectDurationSpec(OwnedTags, 30,
			//		EGAAttributeMod::Add, TEXT("Stamina"), EGAEffectStacking::Override,
			//		AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass());
			//	UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
			//	float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
			//	TestEqual("Source Stamina Post2: ", PostVal2, 180.0f);
			//}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina PreFinished: ", PreFinishedVal, 150.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Finished: ", FinishedVal, 150.0f);
		FGAEffectContext Context = UGABlueprintLibrary::MakeContext(DestActor, SourceActor, DestActor, SourceActor, FHitResult(ForceInit));
		DestComponent->RemoveEffect(Effect, Context);
		float PostRemovedVal = DestComponent->GetAttributeValue(FGAAttribute("Stamina"));
		TestEqual("Source Stamina Finished: ", PostRemovedVal, 100.0f);
	}

	void Test_EffectStatckingOverride()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 5,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationOverride::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 70.0f);
				FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
					UAFPeriodApplicationOverride::StaticClass());
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 32.0f);
	}

	void Test_EffectStatckingOverrideSameClass()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 5,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Override,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationOverride::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 70.0f);
				FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Override,
					AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
					UAFPeriodApplicationOverride::StaticClass());
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 32.0f);
	}

	void Test_EffectStatckingAdd()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 2,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationAdd::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 88.0f);
				FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
					AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
					UAFPeriodApplicationAdd::StaticClass());
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PreFinishedVal: ", PreFinishedVal, 74.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 62.0f);
	}
	
	void Test_EffectStatckingAddSameHandle()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 2,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationAdd::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 88.0f);
				/*FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
					AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
					UAFPeriodApplicationAdd::StaticClass());*/
				UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PreFinishedVal: ", PreFinishedVal, 74.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 62.0f);
	}

	void Test_EffectStatckingAddSameHandleDifferentTargets()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 2,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationAdd::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		//TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("DestComponent Health PPost2: ", PostVal2, 88.0f);
				
				/*FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
				EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Add,
				AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
				UAFPeriodApplicationAdd::StaticClass());*/
				UGABlueprintLibrary::ApplyGameEffectToActor(Effect, TargetOne, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}
			UE_LOG(AbilityFramework, Log, TEXT("Val: %d"), i * 2);
			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("DestComponent Health PreFinishedVal: ", PreFinishedVal, 80.0f);;
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = TargetCompOne->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("TargetCompOne Health Finished: ", FinishedVal, 82.0f);
	}


	void Test_EffectStatckingDurationDifferentEffects()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 1,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Duration,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationOverride::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 94.0f);
				FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Duration,
					AttributeTags, ApplyTags, TagsIn, UGAffectSpecTestOne::StaticClass(),
					UAFPeriodApplicationOverride::StaticClass());
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PreFinishedVal: ", PreFinishedVal, 84.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 72.0f);
	}

	void Test_EffectStatckingDurationSameEffects()
	{
		TArray<FName> OwnedTags;
		OwnedTags.Add("Ability.Fireball");
		TArray<FName> AttributeTags;
		AttributeTags.Add(TEXT("Damage.Fire"));

		TArray<FName> ApplyTags;
		ApplyTags.Add(TEXT("Damage.Fire"));
		ApplyTags.Add(TEXT("Ability.Fireball"));

		FTagsInput TagsIn;

		FGAEffectProperty Effect = CreateEffectPeriodicSpec(OwnedTags, 2,
			EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Duration,
			AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
			UAFPeriodApplicationExtend::StaticClass());

		int32 NumApplications = 0;
		float PreVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Pre: ", PreVal, 100.0f);
		FAFFunctionModifier FuncMod;
		UGABlueprintLibrary::ApplyGameEffectToActor(Effect, DestActor, SourceActor, SourceActor, FuncMod);

		TickWorld(SMALL_NUMBER);
		//++NumApplications;
		float PostVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PPost: ", PostVal, 100.0f);
		const int32 NumPeriods = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TickWorld(PeriodSecs * .1f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			if (i == 5)
			{
				float PostVal2 = DestComponent->GetAttributeValue(FGAAttribute("Health"));
				TestEqual("Source Health PPost2: ", PostVal2, 88.0f);
				FGAEffectProperty EffectWeaker = CreateEffectPeriodicSpec(OwnedTags, 2,
					EGAAttributeMod::Subtract, TEXT("Health"), EGAEffectStacking::Duration,
					AttributeTags, ApplyTags, TagsIn, UGAGameEffectSpec::StaticClass(),
					UAFPeriodApplicationExtend::StaticClass());
				UGABlueprintLibrary::ApplyGameEffectToActor(EffectWeaker, DestActor, SourceActor, SourceActor, FuncMod);
				//TestEqual("Source Health Post2: ", PostVal2, 150.0f);
			}

			// check that health has been reduced

		}
		float PreFinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health PreFinishedVal: ", PreFinishedVal, 80.0f);
		for (int32 i = 0; i < NumPeriods; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
		}
		TickWorld(PeriodSecs * 0.1f);
		float FinishedVal = DestComponent->GetAttributeValue(FGAAttribute("Health"));
		TestEqual("Source Health Finished: ", FinishedVal, 60.0f);
	}

	void Test_StrongerOverrideNonStackingHealthBonus()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			//++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
	}

};
#define ADD_TEST(Name) \
	TestFunctions.Add(&GameEffectsTestSuite::Name); \
	TestFunctionNames.Add(TEXT(#Name))



class FGAAttributesTests : public FAutomationTestBase
{
public:
	typedef void (GameEffectsTestSuite::*TestFunc)();
	TArray<TestFunc> TestFunctions;
	TArray<FString> TestFunctionNames;

	FGAAttributesTests(const FString& InName)
		: FAutomationTestBase(InName, false)
	{
		ADD_TEST(Test_CompareTagContainers);
		ADD_TEST(Test_InstantEffectApplication);
		ADD_TEST(Test_PeriodicDamageEffect);
		ADD_TEST(Test_AtttributeStatckingOverride);
		ADD_TEST(Test_AtttributeStatckingStrongerOverride);
		ADD_TEST(Test_AtttributeStatckingStrongerOverrideReversed);
		ADD_TEST(Test_AtttributeStatckingeAdd);
		ADD_TEST(Test_AtttributeStatckingeAddSameProp);
		ADD_TEST(Test_AtttributeStatckingeAddSamePropDifferentTargets);
		ADD_TEST(Test_AtttributeStatckingeAddInfinite);
		ADD_TEST(Test_EffectStatckingOverride);
		ADD_TEST(Test_EffectStatckingOverrideSameClass);
		ADD_TEST(Test_EffectStatckingAdd);
		ADD_TEST(Test_EffectStatckingAddSameHandle);
		ADD_TEST(Test_EffectStatckingAddSameHandleDifferentTargets);
		ADD_TEST(Test_EffectStatckingDurationDifferentEffects);
		ADD_TEST(Test_EffectStatckingDurationSameEffects);
		ADD_TEST(Test_StrongerOverrideNonStackingHealthBonus);
	};
	virtual uint32 GetTestFlags() const override 
	{
		//EAutomationTestFlags::Type::EditorContext | 
		return (EAutomationTestFlags::Type::EngineFilter);
	}//EAutomationTestFlags::EditorContext | EAutomationTestFlags::SmokeFilter; }
	virtual bool IsStressTest() const { return false; }
	virtual uint32 GetRequiredDeviceNum() const override { return 1; }

	virtual FString GetBeautifiedTestName() const override { return TEXT("GameAttributes.Attributes"); }
	virtual void GetTests(TArray<FString>& OutBeautifiedNames, TArray <FString>& OutTestCommands) const override
	{
		for (const FString& TestFunctionName : TestFunctionNames)
		{
			OutBeautifiedNames.Add(TestFunctionName);
			OutTestCommands.Add(TestFunctionName);
		}
	}
	bool RunTest(const FString& Parameters)
	{
		TestFunc TestFunction = nullptr;
		for (int32 i = 0; i < TestFunctionNames.Num(); ++i)
		{
			if (TestFunctionNames[i] == Parameters)
			{
				TestFunction = TestFunctions[i];
				break;
			}
		}
		if (TestFunction == nullptr)
		{
			return false;
		}
		//UDataTable* TagTable = CreateGameplayDataTable();

		//IGameplayTagsModule::Get().GetGameplayTagsManager().PopulateTreeFromDataTable(TagTable);
		IGameplayTagsModule& GameplayTagsModule = IGameplayTagsModule::Get();
		TArray<FString> TagsList;
		
		FString ItemTags = "/Game/Data/GameplayTags/DamageStatusTags.DamageStatusTags";
		TagsList.Add(ItemTags);
		
		UGameplayTagsManager::Get().DestroyGameplayTagTree();
		//UGameplayTagsManager::Get().LoadGameplayTagTables();
		UGameplayTagsManager::Get().ConstructGameplayTagTree();

		UWorld *World = UWorld::CreateWorld(EWorldType::Game, false);
		FWorldContext &WorldContext = GEngine->CreateNewWorldContext(EWorldType::Game);
		WorldContext.SetCurrentWorld(World);

		FURL URL;
		World->InitializeActorsForPlay(URL);
		World->BeginPlay();

		// run the matching test
		uint64 InitialFrameCounter = GFrameCounter;
		{
			GameEffectsTestSuite Tester(World, this);
			(Tester.*TestFunction)();
		}
		GFrameCounter = InitialFrameCounter;

		GEngine->DestroyWorldContext(World);
		World->DestroyWorld(false);
		return true;
	}
};

namespace AbilityFrameworkTests
{
	FGAAttributesTests FGAAttributesTestsAutomationTestInstance(TEXT("FGAAttributesTests"));
}

#endif