// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "../GameAttributes.h"
#include "AutomationTest.h"
#include "GameplayTagsModule.h"
#include "../GAGlobalTypes.h"
#include "../GAAttributeBase.h"
#include "../GAGameEffect.h"
#include "../GAAttributeComponent.h"
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
	UGAAttributeComponent* SourceComponent;

	AGACharacterAttributeTest* DestActor;
	UGAAttributeComponent* DestComponent;

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

		SourceComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute();
		SourceComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute();

		DestActor = World->SpawnActor<AGACharacterAttributeTest>();
		DestComponent = DestActor->Attributes;
		DestComponent->DefaultAttributes = NewObject<UGAAttributesTest>(SourceActor->Attributes);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetBaseValue(100);
		DestComponent->GetAttributes<UGAAttributesTest>()->Health.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.SetClampValue(500);
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.SetClampValue(500);

		DestComponent->GetAttributes<UGAAttributesTest>()->Health.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->Energy.InitializeAttribute();
		DestComponent->GetAttributes<UGAAttributesTest>()->Stamina.InitializeAttribute();
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
		const float step = 0.1f;
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
		return IGameplayTagsModule::Get().GetGameplayTagsManager().RequestGameplayTag(TagIn);
	}

	FGameplayTagContainer CreateTags(const TArray<FName>& TagsIn)
	{
		FGameplayTagContainer OutTags;
		for (const FName& Tag : TagsIn)
		{
			FGameplayTag ReqTeg = IGameplayTagsModule::Get().GetGameplayTagsManager().RequestGameplayTag(Tag);
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
	
	FGAEffectSpec CreateEffectModifer(const TArray<FName>& Tags, float ModValue,
		EGAAttributeMod ModType, EGAModifierDirection Direction)
	{
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Instant;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		FGAAttributeModifier AttributeModifier;
		FGAGameEffectModifier EffectModifier;
		EffectModifier.Value = ModValue;
		EffectModifier.ModType = ModType;
		EffectModifier.Direction = Direction;
		EffectModifier.RequiredTags = CreateTags(Tags);
		Spec.Spec->Modifiers.Add(EffectModifier);
		FGAGameEffectHandle Handle;
		//UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);
		return Spec;
	}

	FGAEffectSpec CreateAttributeModifer(const TArray<FName>& OwnedTags, float ModValue,
		EGAAttributeMod ModType, const FName& Attribute)
	{
		FGAEffectSpec Spec;
		Spec.Spec = NewObject<UGAGameEffectSpec>(GetTransientPackage());
		Spec.Spec->EffectType = EGAEffectType::Instant;
		Spec.Spec->ExecutionType = UGASpellExecutionTest::StaticClass();
		FGAAttributeModifier AttributeModifier;
		AttributeModifier.Attribute = FGAAttribute(Attribute);
		AttributeModifier.AttributeMod = ModType;
		AttributeModifier.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.DirectModifier.Value = ModValue;
		Spec.Spec->OwnedTags = CreateTags(OwnedTags);
		Spec.Spec->AtributeModifiers.Add(AttributeModifier);
		FGAGameEffectHandle Handle;
		//UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);
		return Spec;
	}

	void ApplyEffectModifiers()
	{

	}
	void Test_EffectModifiers()
	{
		TArray<FName> DamageTags;
		DamageTags.Add(TEXT("Damage.Spell.Fire"));
		FGAEffectSpec FireDamageBuff = CreateEffectModifer(DamageTags, 20, EGAAttributeMod::Add,
			EGAModifierDirection::Outgoing);
		FGAGameEffectHandle FireDamaqgeBuffHandle;
		UGABlueprintLibrary::ApplyGameEffectToActor(FireDamageBuff, FireDamaqgeBuffHandle,
			SourceActor, SourceActor, SourceActor);

		FGAEffectSpec FireDamageDebuff = CreateEffectModifer(DamageTags, 10, EGAAttributeMod::Subtract,
			EGAModifierDirection::Incoming);
		FGAGameEffectHandle FireDamaqgeDebuffHandle;
		UGABlueprintLibrary::ApplyGameEffectToActor(FireDamageDebuff, FireDamaqgeDebuffHandle,
			DestActor, DestActor, DestActor);

		FGAEffectSpec FireDamageEffect = CreateAttributeModifer(DamageTags, 20, EGAAttributeMod::Subtract,
			TEXT("Health"));
		FGAGameEffectHandle FireDamaqgeEffectHandle;
		UGABlueprintLibrary::ApplyGameEffectToActor(FireDamageEffect, FireDamaqgeEffectHandle,
			DestActor, SourceActor, SourceActor);

		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();
		float value = Attributes->Health.GetCurrentValue();
		TestEqual(TEXT("Attribute Base Value"), value, 70);
	}
	void Test_CompareTags()
	{
		FGameplayTag ReqTeg = IGameplayTagsModule::Get().GetGameplayTagsManager().RequestGameplayTag(TEXT("Damage.Spell.Fire"));
		TestEqualTags("Compare Tags", TEXT("Damage.Spell.Fire"), ReqTeg.GetTagName());
	}
	void Test_EffectModifiers2()
	{

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
		AttributeModifier.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.DirectModifier.Value = 20;
		Spec.Spec->AtributeModifiers.Add(AttributeModifier);
		FGAGameEffectHandle Handle;
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
		Spec.Spec->Duration = Duration;
		Spec.Spec->Period = PeriodSecs;
		FGAAttributeModifier AttributeModifier;

		AttributeModifier.Attribute = FGAAttribute("Health");
		AttributeModifier.AttributeMod = EGAAttributeMod::Subtract;
		AttributeModifier.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.DirectModifier.Value = 5;
		Spec.Spec->AtributeModifiers.Add(AttributeModifier);
		FGAGameEffectHandle Handle;
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
		Spec.Spec->Duration = Duration;
		Spec.Spec->Period = PeriodSecs;
		FGAAttributeModifier AttributeModifier;

		AttributeModifier.Attribute = FGAAttribute("Health");
		AttributeModifier.AttributeMod = EGAAttributeMod::Add;
		AttributeModifier.CalculationType = EGAMagnitudeCalculation::Direct;
		AttributeModifier.DirectModifier.Value = 50;
		Spec.Spec->AtributeModifiers.Add(AttributeModifier);
		FGAGameEffectHandle Handle;
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
		Spec.Spec->EffectStacking = EGAEffectStacking::Override;
		Spec.Spec->ExecutionType = UGAEffectExecution::StaticClass();
		Spec.Spec->Duration = Duration;
		Spec.Spec->Period = PeriodSecs;
		TArray<FName> TagsToAdd;
		TagsToAdd.Add(TEXT("Damage.Spell.Fire"));
		FGameplayTagContainer Tags = CreateTags(TagsToAdd);
		Spec.Spec->ApplyTags = Tags;
		FGAGameEffectHandle Handle;
		UGABlueprintLibrary::ApplyGameEffectToActor(Spec, Handle, DestActor, SourceActor, SourceActor);

		UGAAttributesTest* Attributes = DestComponent->GetAttributes<UGAAttributesTest>();

		int32 NumApplications = 0;
		float valueBefore = Attributes->Health.GetCurrentValue();
		// Tick a small number to verify the application tick
		bool HaveTag = DestComponent->AppliedTags.HasAllTags(Tags, false);
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
		
		bool HaveTag1 = DestComponent->AppliedTags.HasAllTags(Tags, false);
		Test->TestTrue(FString("Target Have Tag"), !HaveTag1);
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
		ADD_TEST(Test_EffectModifiers2);
		ADD_TEST(Test_EffectModifiers);
		ADD_TEST(Test_CompareTags);
		ADD_TEST(Test_SetBaseValue);
		ADD_TEST(Test_ApplySimpleEffect);
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
		for (const FString& TestName : TestFunctionNames)
		{
			OutBeautifiedNames.Add(TestName);
			OutTestCommands.Add(TestName);
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

		GameplayTagsModule.GetGameplayTagsManager().DestroyGameplayTagTree();
		GameplayTagsModule.GetGameplayTagsManager().LoadGameplayTagTables(TagsList);
		GameplayTagsModule.GetGameplayTagsManager().ConstructGameplayTagTree();

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