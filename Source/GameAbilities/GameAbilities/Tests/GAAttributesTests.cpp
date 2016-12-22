// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../GameAbilities.h"
#include "AutomationTest.h"
#include "GameplayTagsModule.h"
#include "../GAGlobalTypes.h"
#include "../GAAttributeBase.h"
#include "../GAGameEffect.h"
#include "../GAAbilitiesComponent.h"
#include "../GAAttributesBase.h"
#include "../GAEffectExecution.h"
#include "../Effects/GABlueprintLibrary.h"
#include "GAAttributesTest.h"
#include "GASpellExecutionTest.h"
#include "GACharacterAttributeTest.h"
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

class GameEffectsTestSuite
{
	UWorld* World;
	FAutomationTestBase* Test;

	AGACharacterAttributeTest* SourceActor;
	UGAAbilitiesComponent* SourceComponent;

	AGACharacterAttributeTest* DestActor;
	UGAAbilitiesComponent* DestComponent;

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
		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.SetClampValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.SetClampValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetClampValue(500);
		
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetClampValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetClampValue(500);
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.SetClampValue(500);

		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute();

		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute();

		DestActor = World->SpawnActor<AGACharacterAttributeTest>();
		DestComponent = DestActor->Attributes;
		DestComponent->DefaultAttributes = NewObject<UGAAttributesTest>(SourceActor->Attributes);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetClampValue(500);

		DestComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.SetClampValue(500);

		DestComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute();

		DestComponent->GetAttributes<UGAAttributesTest>()->MagicalBonus.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->PhysicalBonus.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->MagicResistance.InitializeAttribute();
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
		for (const FName& Tag : TagsIn)
		{
			FGameplayTag ReqTeg = UGameplayTagsManager::Get().RequestGameplayTag(Tag);
			if (ReqTeg.IsValid())
			{
				OutTags.AddTag(ReqTeg);
			}
		}
		return OutTags;
	}

	void TestEqual(const FString& TestText, float Actual, float Expected)
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

	FGAEffectSpec CreateEffectSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute)
	{
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Instant;
		Spec.Spec->ExecutionType = UGASpellExecutionTest::StaticClass();
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		Spec.Spec->OwnedTags = CreateTags(OwnedTags);
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAEffectHandle Handle;
		//UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);
		return Spec;
	}
	FGAEffectSpec CreateEffectDurationSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, EGAEffectStacking Stacking)
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Duration;
		Spec.Spec->EffectStacking = Stacking;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		Spec.Spec->OwnedTags = CreateTags(OwnedTags);
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = ModValue;
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = Duration;
		Spec.Spec->Duration = DurationMag;
		return Spec;
	}

	FGAEffectSpec CreateEffectInfiniteSpec(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute, EGAEffectStacking Stacking)
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Infinite;
		Spec.Spec->EffectStacking = Stacking;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		Spec.Spec->OwnedTags = CreateTags(OwnedTags);
		return Spec;
	}

	void ApplyEffectModifiers()
	{

	}
	void Test_CompareTags()
	{
		FGameplayTag ReqTeg = UGameplayTagsManager::Get().RequestGameplayTag(TEXT("Damage.Spell.Fire"));
		TestEqualTags("Compare Tags", TEXT("Damage.Spell.Fire"), ReqTeg.GetTagName());
	}
	void Test_SetBaseValue()
	{
		float value = SourceComponent->GetAttributes<UGAAttributesTest>()->Health.GetFinalValue();
		TestEqual(TEXT("Attribute Base Value"), value, 100);
	}
	void Test_ApplySimpleEffect()
	{
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Instant;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAAttributeModifier AttributeModifier;

		AttributeModifier.Attribute = FGAAttribute("Health");
		AttributeModifier.AttributeMod = EGAAttributeMod::Subtract;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = 20;
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);
		
		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		float value = Attributes->Health.GetCurrentValue();
		TestEqual(TEXT("Attribute Base Value"), value, 80);
	}

	void Test_PeriodicEffect()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Periodic;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = Duration;
		Spec.Spec->Duration = DurationMag;
		FGAMagnitude PeriodMag;
		PeriodMag.CalculationType = EGAMagnitudeCalculation::Direct;
		PeriodMag.DirectModifier.Value = PeriodSecs;
		Spec.Spec->Period = PeriodMag;
		FGAAttributeModifier AttributeModifier;

		AttributeModifier.Attribute = FGAAttribute("Health");
		AttributeModifier.AttributeMod = EGAAttributeMod::Subtract;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = 5;
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();

		int32 NumApplications = 0;

		// Tick a small number to verify the application tick
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);
		float valueBefore = Attributes->Health.GetCurrentValue();
		TestEqual(TEXT("Health Reduced before loop"), valueBefore, 100 - (DamagePerPeriod * NumApplications));

		// Tick a bit more to address possible floating point issues
		

		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;
			float value = Attributes->Health.GetCurrentValue();
			// check that health has been reduced
			TestEqual(TEXT("Health Reduced during tick"), value, 100 - (DamagePerPeriod * NumApplications));
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
		float value = Attributes->Health.GetCurrentValue();
		TestEqual(TEXT("Attribute Final Value"), value, 50);
	}

	void Test_DurationAddEffect()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Duration;
		Spec.Spec->EffectStacking = EGAEffectStacking::Override;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = 10;
		Spec.Spec->Duration = DurationMag;
//		Spec.Spec->Period = PeriodSecs;
		FGAAttributeModifier AttributeModifier;

		AttributeModifier.Attribute = FGAAttribute("Health");
		AttributeModifier.AttributeMod = EGAAttributeMod::Add;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = 50;
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();

		int32 NumApplications = 0;
		float valueBefore = Attributes->Health.GetCurrentValue();
		// Tick a small number to verify the application tick
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);
		
		TestEqual(TEXT("Health Reduced before loop"), valueBefore, 150);

		// Tick a bit more to address possible floating point issues


		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
		float value = Attributes->Health.GetCurrentValue();
		TestEqual(TEXT("Attribute Final Value"), value, 100);
	}

	void Test_AddTagForDuration()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Duration;
		Spec.Spec->EffectStacking = EGAEffectStacking::Add;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		TArray<FName> TagsIn1;
		TagsIn1.Add(TEXT("Damage"));
		TagsIn1.Add(TEXT("Damage.Fire"));
		TagsIn1.Add(TEXT("Damage.Ice"));
		Spec.Spec->OwnedTags = CreateTags(TagsIn1);
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = 10;
		Spec.Spec->Duration = DurationMag;
		//Spec.Spec->Period = PeriodSecs;
		TArray<FName> TagsToAdd;
		TagsToAdd.Add(TEXT("Damage.Spell.Fire"));
		FGameplayTagContainer Tags = CreateTags(TagsToAdd);
		Spec.Spec->ApplyTags = Tags;
		FGAEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);


		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();

		int32 NumApplications = 0;
		float valueBefore = Attributes->Health.GetCurrentValue();
		// Tick a small number to verify the application tick
		bool HaveTag = DestComponent->AppliedTags.HasAll(Tags);
		Test->TestTrue(FString("Target Have Tag"), HaveTag);
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues


		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
		float value = Attributes->Health.GetCurrentValue();
		
		bool HaveTag1 = DestComponent->AppliedTags.HasAll(Tags);
		Test->TestTrue(FString("Target Have Tag"), !HaveTag1);
	}
	void Test_CheckIfStronger()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TArray<FName> OwnedTags1;
		OwnedTags1.Add(TEXT("Damage.Electricity"));
		OwnedTags1.Add(TEXT("Damage.Ice"));
		OwnedTags1.Add(TEXT("Damage.Spell.Fire"));
		FGAEffectSpec Spec = CreateEffectDurationSpec(OwnedTags1, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);

		FGAEffectHandle Handle;
		Handle = UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);
		
		TArray<FName> OwnedTags2;
		OwnedTags2.Add(TEXT("Damage.Electricity"));
		OwnedTags2.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec2 = CreateEffectDurationSpec(OwnedTags2, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);

		FGAEffectHandle Handle2;
		Handle2 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec2, Handle2, DestActor, SourceActor, SourceActor);

		
		int32 NumApplications = 0;
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues
		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		bool bIsStronger = Attributes->MagicalBonus.CheckIfStronger(Handle2);

		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
	}

	void Test_OverrideStacking()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TArray<FName> OwnedTags1;
		OwnedTags1.Add(TEXT("Damage.Electricity"));
		OwnedTags1.Add(TEXT("Damage.Ice"));
		OwnedTags1.Add(TEXT("Damage.Spell.Fire"));
		FGAEffectSpec Spec = CreateEffectDurationSpec(OwnedTags1, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);

		FGAEffectHandle Handle;
		Handle = UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags2;
		OwnedTags2.Add(TEXT("Damage.Electricity"));
		OwnedTags2.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec2 = CreateEffectDurationSpec(OwnedTags2, 20, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);

		FGAEffectHandle Handle2;
		Handle2 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec2, Handle2, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags3;
		OwnedTags3.Add(TEXT("Damage.Earth"));
		OwnedTags3.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec3 = CreateEffectDurationSpec(OwnedTags3, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);

		FGAEffectHandle Handle3;
		Handle3 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec3, Handle3, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags4;
		OwnedTags4.Add(TEXT("Damage.Electricity"));
		OwnedTags4.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec4 = CreateEffectDurationSpec(OwnedTags4, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);
		FGAEffectHandle Handle4;
		Handle4 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec4, Handle4, DestActor, SourceActor, SourceActor);

		int32 NumApplications = 0;
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues
		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		bool bIsStronger = Attributes->MagicalBonus.CheckIfStronger(Handle2);
		FGameplayTagContainer TagTest = CreateTags(OwnedTags2);
		FGAIndividualMods ModsOut;
		float valueBefore = Attributes->MagicalBonus.GetBonusValueByTags(TagTest, ModsOut);
			TestEqual(TEXT("Damage.Electricity Bonus:"), valueBefore, 10);

		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
	}

	void Test_GetBonusByTag()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Duration;
		Spec.Spec->EffectStacking = EGAEffectStacking::Add;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute("MagicalBonus");
		AttributeModifier.AttributeMod = EGAAttributeMod::Add;
		AttributeModifier.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.Magnitude.DirectModifier.Value = 10;
		Spec.Spec->AtributeModifier = AttributeModifier;
		FGAMagnitude DurationMag;
		DurationMag.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag.DirectModifier.Value = 10;
		Spec.Spec->Duration = DurationMag;
		//Spec.Spec->Period = PeriodSecs;
		TArray<FName> TagsToAdd;
		TagsToAdd.Add(TEXT("Damage.Spell.Fire"));
		FGameplayTagContainer Tags = CreateTags(TagsToAdd);
		Spec.Spec->ApplyTags = Tags;
		FGAEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);


		FGAEffectSpec Spec2;
		Spec2.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec2.Spec->EffectType = EGAEffectType::Duration;
		Spec2.Spec->EffectStacking = EGAEffectStacking::Add;
		Spec2.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAAttributeModifier AttributeModifier2;
		AttributeModifier2.Attribute = FGAAttribute("MagicalBonus");
		AttributeModifier2.AttributeMod = EGAAttributeMod::Add;
		AttributeModifier2.Magnitude.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier2.Magnitude.DirectModifier.Value = 15;
		Spec2.Spec->AtributeModifier = AttributeModifier2;
		TArray<FName> TagsIn2;
		TagsIn2.Add(TEXT("Damage.Electricity"));
		TagsIn2.Add(TEXT("Damage.Ice"));
		TagsIn2.Add(TEXT("Damage.Spell.Fire"));
		Spec2.Spec->OwnedTags = CreateTags(TagsIn2);
		FGAMagnitude DurationMag2;
		DurationMag2.CalculationType = EGAMagnitudeCalculation::Direct;
		DurationMag2.DirectModifier.Value = 10;
		Spec2.Spec->Duration = DurationMag2;
		//Spec.Spec->Period = PeriodSecs;
		TArray<FName> TagsToAdd2;
		TagsToAdd.Add(TEXT("Damage.Spell.Fire"));
		FGameplayTagContainer Tags2 = CreateTags(TagsToAdd2);
		Spec2.Spec->ApplyTags = Tags2;
		FGAEffectHandle Handle2;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec2, Handle2, DestActor, SourceActor, SourceActor);

		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();

		int32 NumApplications = 0;
		TArray<FName> TagsIn3;
		TagsIn3.Add(TEXT("Damage.Electricity"));
		FGameplayTagContainer TagTest = CreateTags(TagsIn3);
		FGAIndividualMods Out;
		float valueBefore = Attributes->MagicalBonus.GetBonusValueByTags(TagTest, Out);
		// Tick a small number to verify the application tick
		bool HaveTag = DestComponent->AppliedTags.HasAll(Tags);
		TestEqual(TEXT("Damage.Electricity Bonus:"), valueBefore, 15);
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues


		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
		float value = Attributes->Health.GetCurrentValue();

		bool HaveTag1 = DestComponent->AppliedTags.HasAll(Tags);
	}

	void Test_CompareTagContainers()
	{
		TArray<FName> TagsA;
		TagsA.Add(TEXT("Damage.Fire"));
		TagsA.Add(TEXT("Damage.Ice"));
		TagsA.Add(TEXT("Damage.Acid"));
		FGameplayTagContainer A = CreateTags(TagsA);

		TArray<FName> TagsB;
		TagsB.Add(TEXT("Damage.Fire"));
		TagsB.Add(TEXT("Damage.Ice"));
		FGameplayTagContainer B = CreateTags(TagsB);

		bool AB = A.HasAll(B);
		bool BA = B.HasAll(A);

		float dd = 0;
	}

	void Test_CalculateSpellDamage()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TArray<FName> OwnedTags1;
		OwnedTags1.Add(TEXT("Damage.Electricity"));
		OwnedTags1.Add(TEXT("Damage.Ice"));
		OwnedTags1.Add(TEXT("Damage.Spell.Fire"));
		FGAEffectSpec Spec = CreateEffectDurationSpec(OwnedTags1, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);

		FGAEffectHandle Handle;
		Handle = UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags2;
		OwnedTags2.Add(TEXT("Damage.Electricity"));
		OwnedTags2.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec2 = CreateEffectDurationSpec(OwnedTags2, 20, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);

		FGAEffectHandle Handle2;
		Handle2 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec2, Handle2, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags3;
		OwnedTags3.Add(TEXT("Damage.Earth"));
		OwnedTags3.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec3 = CreateEffectDurationSpec(OwnedTags3, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);

		FGAEffectHandle Handle3;
		Handle3 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec3, Handle3, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags4;
		OwnedTags4.Add(TEXT("Damage.Electricity"));
		OwnedTags4.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec4 = CreateEffectDurationSpec(OwnedTags4, 10, EGAAttributeMod::Subtract
			, TEXT("MagicalBonus"), EGAEffectStacking::Add);
		FGAEffectHandle Handle4;
		Handle4 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec4, Handle4, DestActor, SourceActor, SourceActor);

		int32 NumApplications = 0;
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues
		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		bool bIsStronger = Attributes->MagicalBonus.CheckIfStronger(Handle2);
		FGameplayTagContainer TagTest = CreateTags(OwnedTags2);
		FGAIndividualMods ModsOut;
		float valueBefore = Attributes->MagicalBonus.GetBonusValueByTags(TagTest, ModsOut);
		TestEqual(TEXT("Damage.Electricity Bonus:"), valueBefore, 20);

		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
		}

		// advance time by one extra period
		TickWorld(PeriodSecs);
	}

	void Test_OverrideSimiliarTags()
	{
		const float Duration = 10;
		const float PeriodSecs = 1.0f;
		const float DamagePerPeriod = 5.f;
		TArray<FName> OwnedTags1;
		OwnedTags1.Add(TEXT("Damage.Electricity"));
		OwnedTags1.Add(TEXT("Damage.Ice"));
		//OwnedTags1.Add(TEXT("Damage.Spell.Fire"));
		FGAEffectSpec Spec = CreateEffectDurationSpec(OwnedTags1, 10, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);

		FGAEffectHandle Handle;
		Handle = UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		TArray<FName> OwnedTags2;
		OwnedTags2.Add(TEXT("Damage.Electricity"));
		OwnedTags2.Add(TEXT("Damage.Ice"));
		FGAEffectSpec Spec2 = CreateEffectDurationSpec(OwnedTags2, 20, EGAAttributeMod::Add
			, TEXT("MagicalBonus"), EGAEffectStacking::Override);

		FGAEffectHandle Handle2;
		Handle2 = UGABlueprintLibrary::ApplyGameEffectToActor(Spec2, Handle2, DestActor, SourceActor, SourceActor);

		int32 NumApplications = 0;
		TickWorld(SMALL_NUMBER);
		++NumApplications; //possible problem with effect or test ?
		TickWorld(PeriodSecs * .1f);

		// Tick a bit more to address possible floating point issues
		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		FGameplayTagContainer TagTest = CreateTags(OwnedTags1);
		FGAIndividualMods ModsOut;
		float valueBefore = Attributes->MagicalBonus.GetBonusValueByTags(TagTest, ModsOut);
		int32 modCount = Attributes->MagicalBonus.Modifiers.Num();
		TestEqual(TEXT("Damage.Electricity Bonus:"), valueBefore, 10);
		TestEqual(TEXT("MagicalBonus modifier count:"), modCount, 2);

		for (int32 i = 0; i < 9; ++i)
		{
			// advance time by one period
			TickWorld(PeriodSecs);
			++NumApplications;;
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
		ADD_TEST(Test_AddTagForDuration);
		ADD_TEST(Test_DurationAddEffect);
		ADD_TEST(Test_PeriodicEffect);
		ADD_TEST(Test_CompareTags);
		ADD_TEST(Test_SetBaseValue);
		ADD_TEST(Test_ApplySimpleEffect);
		ADD_TEST(Test_GetBonusByTag);
		ADD_TEST(Test_CheckIfStronger);
		ADD_TEST(Test_OverrideStacking);
		ADD_TEST(Test_CompareTagContainers);
		ADD_TEST(Test_CalculateSpellDamage);
		ADD_TEST(Test_OverrideSimiliarTags);
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
		UDataTable* TagTable = CreateGameplayDataTable();

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

namespace
{
	FGAAttributesTests FGAAttributesTestsAutomationTestInstance(TEXT("FGAAttributesTests"));
}

#endif