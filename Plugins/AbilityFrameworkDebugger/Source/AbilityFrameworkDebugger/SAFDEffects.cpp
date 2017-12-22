// Fill out your copyright notice in the Description page of Project Settings.

#include "SAFDEffects.h"
#include "SlateOptMacros.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SAFDEffects::Construct(const FArguments& InArgs)
{
	AFInterface = InArgs._AbilityInterface;
	AbilityComponent = AFInterface->GetAbilityComp();

	OnEffectAppliedHandle = AbilityComponent->OnEffectAppliedToTarget.AddSP(this, &SAFDEffects::OnEffectApplied);
	OnEffectRemovedHandle = AbilityComponent->OnEffectRemoved.AddSP(this, &SAFDEffects::OnEffectRemoved);
	OnEffectExpiredHandle = AbilityComponent->OnEffectExpired.AddSP(this, &SAFDEffects::OnEffectRemoved);

	InitializeEffects();
	ChildSlot
	[
		SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(EHorizontalAlignment::HAlign_Fill)
		.VAlign(EVerticalAlignment::VAlign_Fill)
		[
			SAssignNew(ListView, SListView<TSharedPtr<FAFDEffectRow>>)
			.ListItemsSource(&Effects)
			.OnGenerateRow(this, &SAFDEffects::GenerateListRow)
		]
	];
	
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
SAFDEffects::~SAFDEffects()
{
	AbilityComponent->OnEffectAppliedToTarget.Remove(OnEffectAppliedHandle);
	AbilityComponent->OnEffectRemoved.Remove(OnEffectRemovedHandle);
	AbilityComponent->OnEffectExpired.Remove(OnEffectExpiredHandle);
}

void SAFDEffects::InitializeEffects()
{
	if (!AbilityComponent.IsValid())
		return;

	for(const FAFEffectRepInfo& RepInfo : AbilityComponent->GetAllEffectsInfo())
	{
		TSharedPtr<FAFDEffectRow> Row = MakeShareable(new FAFDEffectRow);
		Row->Handle = RepInfo.Handle;
		Row->RepInfo = const_cast<FAFEffectRepInfo*>(&RepInfo);
		Row->AbilityComponent = AbilityComponent;
		Row->EffectClassName = RepInfo.Handle.GetEffectSpec()->GetName();
		Row->TimeRemaining = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(Row.Get(), &FAFDEffectRow::GetTimeRemaining));
		Row->PeriodTime = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(Row.Get(), &FAFDEffectRow::GetPeriodTime));
		Effects.Add(Row);
	}
	//ListView->RebuildList();
}


TSharedRef<ITableRow> SAFDEffects::GenerateListRow(TSharedPtr<FAFDEffectRow> EffectRow, const TSharedRef<STableViewBase>& OwnerTable)
{
	return
		SNew(STableRow< TSharedRef<FSoftObjectPath> >, OwnerTable)
		[
			SNew(SGridPanel)
			.FillColumn(0, 1)
			/*.FillColumn(1,1)
			.FillColumn(2,1)*/
			+ SGridPanel::Slot(0, 0)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(FText::FromString(EffectRow->EffectClassName))
			]
			+ SGridPanel::Slot(1, 0)
			.HAlign(EHorizontalAlignment::HAlign_Fill)
			[
				SNew(STextBlock)
				.Text(EffectRow->TimeRemaining)
			]
			+ SGridPanel::Slot(2, 0)
				.HAlign(EHorizontalAlignment::HAlign_Fill)
				[
					SNew(STextBlock)
					.Text(EffectRow->PeriodTime)
				]
		];
}

void SAFDEffects::OnEffectApplied(FGAEffectHandle InHandle)
{
	TSharedPtr<FAFDEffectRow> Row = MakeShareable(new FAFDEffectRow);
	Row->Handle = InHandle;
	Row->RepInfo = AbilityComponent->GameEffectContainer.GetReplicationInfo(InHandle);
	Row->AbilityComponent = AbilityComponent;
	Row->EffectClassName = InHandle.GetEffectSpec()->GetName();
	Row->TimeRemaining = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(Row.Get(), &FAFDEffectRow::GetTimeRemaining));
	Row->PeriodTime = TAttribute<FText>::Create(TAttribute<FText>::FGetter::CreateSP(Row.Get(), &FAFDEffectRow::GetPeriodTime));
	UE_LOG(LogTemp, Warning, TEXT("SAFDEffects::OnEffectAppliede"));
	Effects.Add(Row);
	
	ListView->RebuildList();
}
void SAFDEffects::OnEffectRemoved(FGAEffectHandle InHandle)
{
	class Predicate
	{
		FGAEffectHandle HandleC;
	public:
		Predicate(const FGAEffectHandle& InH)
			:HandleC(InH)
		{}
		bool operator()(const FAFDEffectRow& Other) const
		{
			return HandleC == Other.Handle;
		}
		bool operator()(TSharedPtr<FAFDEffectRow> Other) const
		{
			return HandleC == Other->Handle;
		}
	};
	int32 Idx = Effects.IndexOfByPredicate(Predicate(InHandle)); //Effects.IndexOfByKey<FGAEffectHandle>(InHandle);
	if(Idx > -1)
	{
		Effects.RemoveAt(Idx);
	}

	ListView->RebuildList();
}