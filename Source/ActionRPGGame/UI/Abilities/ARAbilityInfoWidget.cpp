// Fill out your copyright notice in the Description page of Project Settings.

#include "ARAbilityInfoWidget.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "ARUIAbilityManagerComponent.h"
#include "Abilities/GAAbilityBase.h"
#include "ARPlayerController.h"

void UARAbilityInfoWidget::NativePreConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		OwningComponent = MyPC->UIAbilityManagerComponent;
	}
	Super::NativePreConstruct();
}
void UARAbilityInfoWidget::NativeConstruct()
{
	if (AARPlayerController* MyPC = Cast<AARPlayerController>(GetOwningPlayer()))
	{
		OwningComponent = MyPC->UIAbilityManagerComponent;
	}
	Super::NativeConstruct();
}

void UARAbilityInfoWidget::InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent)
{
	OwningComponent = InOwningComponent;
}
void UARAbilityInfoWidget::NativeAddAbility(const FGameplayTag& InAbilityTag)
{
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetOwningPlayer()->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	if (!AbilityComp->OnAbilityAdded.IsAlreadyBound(this, &UARAbilityInfoWidget::OnAbilityReady))
	{
		AbilityComp->OnAbilityAdded.AddDynamic(this, &UARAbilityInfoWidget::OnAbilityReady);
	}
	AbilityTag = InAbilityTag;
	TSubclassOf<UGAAbilityBase> AbilityClass = OwningComponent->AbilityData->Items.FindRef(InAbilityTag).AbilityClass;
	AbilityComp->NativeAddAbility(AbilityClass, nullptr, FGameplayTag(), false);
}
void UARAbilityInfoWidget::NativeOnAbilityReady(const FGameplayTag& InAbilityTag)
{
	if (AbilityTag != InAbilityTag)
	{
		return;
	}
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(GetOwningPlayer()->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	BP_OnAbilityReady(InAbilityTag);
	Ability = AbilityComp->BP_GetAbilityByTag(InAbilityTag);
}
void UARAbilityInfoWidget::AddAbility(const FGameplayTag& InAbilityTag)
{
	NativeAddAbility(InAbilityTag);
}
void UARAbilityInfoWidget::BP_InitializeWidget(UARUIAbilityManagerComponent* InOwningComponent)
{
	InitializeWidget(InOwningComponent);
}
void UARAbilityInfoWidget::OnAbilityReady(const FGameplayTag& InAbilityTag)
{
	NativeOnAbilityReady(InAbilityTag);
}

