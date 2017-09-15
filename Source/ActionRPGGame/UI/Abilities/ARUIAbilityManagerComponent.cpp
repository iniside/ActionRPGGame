// Fill out your copyright notice in the Description page of Project Settings.

#include "ARUIAbilityManagerComponent.h"
#include "ARAbilityInfoWidget.h"
#include "ARWeaponInfoWidget.h"
#include "ARPlayerController.h"
#include "AFAbilityComponent.h"
#include "AssetRegistryModule.h"
#include "Engine/AssetManager.h"
#include "ARAbilityBase.h"
#include "ARAbilityUIData.h"
// Sets default values for this component's properties
UARUIAbilityManagerComponent::UARUIAbilityManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARUIAbilityManagerComponent::BeginPlay()
{
	//initiazile Model (hhueheu).
	AbilitySet.SetNum(2);
	AbilitySet[0].SetNum(6);
	AbilitySet[1].SetNum(6);

	AbilityTagsSet.SetNum(2);
	AbilityTagsSet[0].SetNum(6);
	AbilityTagsSet[1].SetNum(6);

	//InputBindingsSet.SetNum(2);
	//InputBindingsSet[0].SetNum(6);
	//InputBindingsSet[1].SetNum(6);

	ActiveSet = 0;

	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		if (AbilitySetConfigClass)
		{
			AbilitySetConfigWidget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetOwner()), AbilitySetConfigClass);
			//AbilitySetConfigWidget->InitializeWidget(this);
			AbilitySetConfigWidget->AddToViewport();
		}
		if (AbilityWidgetClass)
		{
			AbilityWidget = CreateWidget<UARAbilityInfoWidget>(Cast<APlayerController>(GetOwner()), AbilityWidgetClass);
			//AbilityWidget->InitializeWidget(this);
		}
		if (WeaponWidgetClass)
		{
			WeaponWidget = CreateWidget<UARWeaponInfoWidget>(Cast<APlayerController>(GetOwner()), WeaponWidgetClass);
			//WeaponWidget->InitializeWidget(this);
		}

		if (WeaponCrosshairWidgetClass)
		{
			WeaponCrosshairWidget = CreateWidget<UUserWidget>(Cast<APlayerController>(GetOwner()), WeaponCrosshairWidgetClass);
			WeaponCrosshairWidget->AddToViewport();
		}
	}
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	for (const FGameplayTag& Tag : AbilityInputs)
	{
		AbilityComp->BP_BindAbilityToAction(Tag);
	}
	Super::BeginPlay();
}


// Called every frame
void UARUIAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UARAbilityBase* UARUIAbilityManagerComponent::GetAbility(int32 SetIndex, int32 AbilityIndex)
{
	if(AbilitySet[SetIndex][AbilityIndex].IsValid())
		return AbilitySet[SetIndex][AbilityIndex].Get();
	return nullptr;
}
void UARUIAbilityManagerComponent::SetAbility(int32 SetIndex, int32 AbilityIndex, UARAbilityBase* InAbility)
{
	AbilitySet[SetIndex][AbilityIndex] = InAbility;
}
FGameplayTag UARUIAbilityManagerComponent::GetAbilityTag(int32 SetIndex, int32 AbilityIndex)
{
	return AbilityTagsSet[SetIndex][AbilityIndex];
}
void UARUIAbilityManagerComponent::SetAbilityTag(int32 SetIndex, int32 AbilityIndex, FGameplayTag InAbilityTag)
{
	AbilityTagsSet[SetIndex][AbilityIndex] = InAbilityTag;
}

FGameplayTag UARUIAbilityManagerComponent::GetInputTag(int32 SetIndex, int32 AbilityIndex)
{
	return InputBindingsSet[SetIndex][AbilityIndex];
}
void UARUIAbilityManagerComponent::SetInputTag(int32 SetIndex, int32 AbilityIndex, FGameplayTag InAbilityTag)
{
	InputBindingsSet[SetIndex][AbilityIndex] = InAbilityTag;
}

void UARUIAbilityManagerComponent::NativeEquipAbility(const FGameplayTag& InAbilityTag, int32 InAbilitySet
	, int32 AbilityIndex)
{
	//fake implementation untill I add AssetManager support.
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	/*if (!AbilityComp->OnAbilityAdded.IsAlreadyBound(this, &UARUIAbilityManagerComponent::OnAbilityReady))
	{
		FGameplayTag d = GetInputTag(InAbilitySet, AbilityIndex);
		AbilityComp->OnAbilityReady.AddDynamic(this, &UARUIAbilityManagerComponent::OnAbilityReady);
	}*/
	FGameplayTag IAbilityInput = GetInputTag(InAbilitySet, AbilityIndex);
	FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UARUIAbilityManagerComponent::OnAbilityReady, 
		InAbilityTag, IAbilityInput, InAbilitySet, AbilityIndex);
	AbilityComp->AddOnAbilityReadyDelegate(InAbilityTag, ReadyDelegate);

	AbilityComp->NativeAddAbilityFromTag(InAbilityTag, nullptr);// , GetInputTag(InAbilitySet, AbilityIndex));
	
}
void UARUIAbilityManagerComponent::OnAbilityReady(FGameplayTag InAbilityTag, FGameplayTag InAbilityInput,
	int32 InAbilitySet, int32 InAbilityIndex)
{
	NativeOnAbilityReady(InAbilityTag, InAbilityInput, InAbilitySet, InAbilityIndex);
}
void UARUIAbilityManagerComponent::NativeOnAbilityReady(const FGameplayTag& InAbilityTag, const FGameplayTag InAbilityInput,
	int32 InAbilitySet, int32 InAbilityIndex)
{
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;

	
	UARAbilityBase* Ability = Cast<UARAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
		
	SetAbility(InAbilitySet, InAbilityIndex, Ability);
	SetAbilityTag(InAbilitySet, InAbilityIndex, InAbilityTag);
	SetInputTag(InAbilitySet, InAbilityIndex, InAbilityInput);
		
		
	AbilityComp->SetAbilityToAction(InAbilityTag, InAbilityInput, FAFOnAbilityReady());
}

void UARUIAbilityManagerComponent::SwitchSet()
{
	//in reality it should be vlidated on server as well
	//although this system is independent from ability component.
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return;
	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return;

	UAFAbilityComponent* AbilityComp = ABInt->GetAbilityComp();
	if (!AbilityComp)
		return;


	if (ActiveSet == 0)
	{
		const TArray<FGameplayTag>& AbilityTags = AbilityTagsSet[0];
		const FARAbilityInputBinding& InputTags = InputBindingsSet[0];
		const FARAbilityInputBinding InputTags2 = InputBindingsSet[1];
		for (int32 Idx = 0; Idx < 6; Idx++)
		{
			AbilityComp->SetBlockedInput(InputTags[Idx], true);
			AbilityComp->SetBlockedInput(InputTags2[Idx], false);
			//AbilityComp->BP_BindAbilityToAction(InputBinding, AbilityTag);
		}
		ActiveSet = 1;
		OnAbilitySetChanged.Broadcast(1);
	}
	else if (ActiveSet == 1)
	{
		const TArray<FGameplayTag>& AbilityTags = AbilityTagsSet[1];
		const FARAbilityInputBinding& InputTags = InputBindingsSet[1];
		const FARAbilityInputBinding& InputTags2 = InputBindingsSet[0];
		for (int32 Idx = 0; Idx < 6; Idx++)
		{
			AbilityComp->SetBlockedInput(InputTags[Idx], true);
			AbilityComp->SetBlockedInput(InputTags2[Idx], false);
			//AbilityComp->BP_BindAbilityToAction(InputBinding, AbilityTag);
		}
		ActiveSet = 0;
		OnAbilitySetChanged.Broadcast(0);
	}
}

void UARUIAbilityManagerComponent::FinishedLoadinFiles()
{
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> AssetData;
	FARFilter Filter;
	Filter.ClassNames.Add(TEXT("UGAAbilityBase"));// .Classes.Add(UStaticMesh::StaticClass());
	Filter.TagsAndValues.Add("Ability.Corruption");
	Filter.bRecursiveClasses = true;
	AssetRegistryModule.Get().GetAssets(Filter, AssetData);

	FARFilter Filter2;
	Filter2.bRecursiveClasses = true;
	Filter2.ClassNames.Add(TEXT("UGAAbilityBase"));// .Classes.Add(UStaticMesh::StaticClass());

	TArray<FAssetData> AssetData2;
	AssetRegistryModule.Get().GetAssets(Filter2, AssetData2);

}