// Fill out your copyright notice in the Description page of Project Settings.

#include "AMAbilityManagerComponent.h"
#include "AFAbilityComponent.h"
#include "AFAbilityInterface.h"

// Sets default values for this component's properties
UAMAbilityManagerComponent::UAMAbilityManagerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UAMAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	//OnNextGroupDelegate = FGroupConfirmDelegate::(this, UAMAbilityManagerComponent::OnNextGroupConfirmed);
	// ...
	
}
void UAMAbilityManagerComponent::InitializeComponent()
{
	uint8 GroupNum = Groups.Num();
	
	AbilitySet.SetNum(GroupNum);
	AbilityTagsSet.SetNum(GroupNum);
	for(int32 Idx = 0; Idx < GroupNum; Idx++)
	{
		AbilitySet[Idx].SetNum(Groups[Idx].SlotNum);
		AbilityTagsSet[Idx].SetNum(Groups[Idx].SlotNum);
	}
}

// Called every frame
void UAMAbilityManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}
void UAMAbilityManagerComponent::BindInputs(UInputComponent* InputComponent, class UAFAbilityComponent* AbilityComponent)
{
	if (!AbilityComponent)
		return;

	for (const FGameplayTag& Input : InputsToBind)
	{
		AbilityComponent->BindAbilityToAction(InputComponent, Input);
	}
}
UGAAbilityBase* UAMAbilityManagerComponent::GetAbility(EAMGroup InGroup, EAMSlot InSlot)
{
	return AbilitySet.Num() >= Groups.Num() ? AbilitySet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)].Get() : nullptr;
}
void UAMAbilityManagerComponent::SetAbility(EAMGroup InGroup, EAMSlot InSlot, UGAAbilityBase* InAbility)
{
	if (AbilitySet.Num() < Groups.Num())
		return;

	AbilitySet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)] = InAbility;
}

TArray<FGameplayTag> UAMAbilityManagerComponent::GetInputTag(EAMGroup InGroup, EAMSlot InSlot)
{
	return InputSetup.GetInputs(InGroup, InSlot);
}
void UAMAbilityManagerComponent::SetInputTag(EAMGroup InGroup, EAMSlot InSlot, TArray<FGameplayTag> InAbilityTag)
{

}

TSoftClassPtr<UGAAbilityBase> UAMAbilityManagerComponent::GetAbilityTag(EAMGroup InGroup, EAMSlot InSlot)
{
	if (AbilityTagsSet.IsValidIndex(AMEnumToInt<EAMGroup>(InGroup))
		&& AbilityTagsSet[AMEnumToInt<EAMGroup>(InGroup)].IsValidIndex(AMEnumToInt<EAMSlot>(InSlot)))
	{
		return AbilityTagsSet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)];
	}
	return TSoftClassPtr<UGAAbilityBase>();
}
void UAMAbilityManagerComponent::SetAbilityTag(EAMGroup InGroup, EAMSlot InSlot, TSoftClassPtr<UGAAbilityBase> InAbilityTag)
{
	AbilityTagsSet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)] = InAbilityTag;
}
void UAMAbilityManagerComponent::NativeEquipAbility(const TSoftClassPtr<UGAAbilityBase>& InAbilityTag, EAMGroup InGroup, EAMSlot InSlot, AActor* InAvatar, bool bBindInput)
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
	TArray<FGameplayTag> IAbilityInput;
	
	if(ActiveGroup == InGroup)
		IAbilityInput = GetInputTag(InGroup, InSlot);

	FAFOnAbilityReady del;
	//if (IsClient())
	{
		del = FAFOnAbilityReady::CreateUObject(this, &UAMAbilityManagerComponent::OnAbilityReadyInternal, InAbilityTag,
			IAbilityInput, InGroup, InSlot);
		AbilityComp->AddOnAbilityReadyDelegate(InAbilityTag, del);
	}

	AbilityComp->NativeAddAbility(InAbilityTag, IAbilityInput);// , /*Input*/ ShootInput);
}
void UAMAbilityManagerComponent::OnAbilityReadyInternal(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput,
	EAMGroup InGroup, EAMSlot InSlot)
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


	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &UAMAbilityManagerComponent::OnAbilityInputReady,
			InAbilityTag, InAbilityInput, InGroup, InSlot);

		AbilityComp->SetAbilityToAction(InAbilityTag, InAbilityInput, ReadyDelegate);
		ExecuteAbilityReadyEvent(InAbilityTag);
	}
	else
	{
		SetAbility(InGroup, InSlot, Ability);
		SetAbilityTag(InGroup, InSlot, InAbilityTag);
		SetInputTag(InGroup, InSlot, InAbilityInput);
		AbilityComp->SetAbilityToAction(InAbilityTag, InAbilityInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(InAbilityTag);
	}
	OnAbilityReady(InAbilityTag, InAbilityInput, InGroup, InSlot);
}

void UAMAbilityManagerComponent::OnAbilityInputReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput,
	EAMGroup InGroup, EAMSlot InSlot)
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
	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(AbilityComp->BP_GetAbilityByTag(InAbilityTag));
	SetAbility(InGroup, InSlot, Ability);
	SetAbilityTag(InGroup, InSlot, InAbilityTag);
	SetInputTag(InGroup, InSlot, InAbilityInput);
	//ExecuteAbilityReadyEvent(InAbilityTag);
}

void UAMAbilityManagerComponent::NextGroup()
{
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
		CurrentIndex++;
		if(CurrentIndex > Groups.Num())
		{
			ActiveGroup = EAMGroup::Group001;
		}
		else
		{
			ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
		}
		
		if (ActiveGroup < AMIntToEnum<EAMGroup>(Groups.Num()))
		{
		}
		else
		{
			ActiveGroup = EAMGroup::Group001;
		}
	}

	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup));
	}
}

void UAMAbilityManagerComponent::PreviousGroup()
{
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	CurrentIndex--;
	ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	if (CurrentIndex < 0)
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(Groups.Num() - 1);
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerPreviousGroup(static_cast<int32>(ActiveGroup));
	}
}

void UAMAbilityManagerComponent::ServerNextGroup_Implementation(int32 WeaponIndex)
{
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	CurrentIndex++;
	ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	if (CurrentIndex > Groups.Num())
	{
		ActiveGroup = EAMGroup::Group001;
	}
	else
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	}
	if (ActiveGroup < AMIntToEnum<EAMGroup>(Groups.Num()))
	{
	}
	else
	{
		ActiveGroup = EAMGroup::Group001;
	}
	//so Server index is different. Client might tried to cheat
	//or sometrhing. We will override it.
	//situation where client can chage multiple weapons within second
	//should not have place, as there is animation and/or internal cooldown on weapon change.
	//since it will be done trough ability.
	if (ActiveGroup != AMIntToEnum<EAMGroup>(WeaponIndex))
	{
		ClientNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup), false);
	}
	else
	{
		ClientNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup), true);
	}
}
bool UAMAbilityManagerComponent::ServerNextGroup_Validate(int32 WeaponIndex)
{
	return true;
}
void UAMAbilityManagerComponent::ClientNextGroup_Implementation(int32 WeaponIndex, bool bPredictionSuccess)
{
	ActiveGroup = AMIntToEnum<EAMGroup>(WeaponIndex);
	OnNextGroupConfirmed(ActiveGroup, bPredictionSuccess);
}

void UAMAbilityManagerComponent::ServerPreviousGroup_Implementation(int32 WeaponIndex)
{
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	CurrentIndex--;
	ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	if (CurrentIndex < 0)
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(Groups.Num() -1);
	}
	if (ActiveGroup != AMIntToEnum<EAMGroup>(WeaponIndex))
	{
		ClientPreviousGroup(AMEnumToInt<EAMGroup>(ActiveGroup), false);
	}
	else
	{
		ClientPreviousGroup(AMEnumToInt<EAMGroup>(ActiveGroup), true);
	}
}
bool UAMAbilityManagerComponent::ServerPreviousGroup_Validate(int32 WeaponIndex)
{
	return true;
}
void UAMAbilityManagerComponent::ClientPreviousGroup_Implementation(int32 WeaponIndex, bool bPredictionSuccess)
{
	ActiveGroup = AMIntToEnum<EAMGroup>(WeaponIndex);
	OnPreviousGroupConfirmed(ActiveGroup, bPredictionSuccess);
}

void UAMAbilityManagerComponent::SelectGroup(EAMGroup InGroup)
{
	if (AMEnumToInt<EAMGroup>(InGroup) > Groups.Num())
	{
		ActiveGroup = EAMGroup::Group001;
	}
	else
	{
		ActiveGroup = InGroup;
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerSelectGroup(InGroup);
	}
	else
	{
		OnGroupSelectionConfirmed(InGroup, true);
	}
}
void UAMAbilityManagerComponent::ServerSelectGroup_Implementation(EAMGroup InGroup)
{
	if (AMEnumToInt<EAMGroup>(InGroup) > Groups.Num())
	{
		ActiveGroup = EAMGroup::Group001;
	}
	else
	{
		ActiveGroup = InGroup;
	}
	if (ActiveGroup != InGroup)
	{
		ClientPreviousGroup(AMEnumToInt<EAMGroup>(ActiveGroup), false);
	}
	else
	{
		ClientPreviousGroup(AMEnumToInt<EAMGroup>(ActiveGroup), true);
	}
}
bool UAMAbilityManagerComponent::ServerSelectGroup_Validate(EAMGroup InGroup)
{
	return true;
}


void UAMAbilityManagerComponent::ClientSelectGroup_Implementation(EAMGroup InGroup, bool bPredictionSuccess)
{
	if (bPredictionSuccess)
	{
		OnGroupSelectionConfirmed(InGroup, true);
	}
	else
	{
		ActiveGroup = InGroup;
		OnGroupSelectionConfirmed(InGroup, false);
	}
}


class UAFAbilityComponent* UAMAbilityManagerComponent::GetAbilityComponent()
{
	UAFAbilityComponent* AbilityComponent = nullptr;
	APlayerController* MyPC = Cast<APlayerController>(GetOwner());
	if (!MyPC)
		return AbilityComponent;

	IAFAbilityInterface* ABInt = Cast<IAFAbilityInterface>(MyPC->GetPawn());
	if (!ABInt)
		return AbilityComponent;

	AbilityComponent = ABInt->GetAbilityComp();
	
	return AbilityComponent;
}

bool UAMAbilityManagerComponent::IsServerOrStandalone() const
{
	AActor* Owner = GetOwner();
	if (Owner->GetNetMode() == ENetMode::NM_DedicatedServer
		|| Owner->GetNetMode() == ENetMode::NM_Standalone)
	{
		return true;
	}
	return false;
}
bool UAMAbilityManagerComponent::IsClientOrStandalone() const
{
	AActor* Owner = GetOwner();
	if (Owner->GetNetMode() == ENetMode::NM_Client
		|| Owner->GetNetMode() == ENetMode::NM_Standalone)
	{
		return true;
	}
	return false;
}
bool UAMAbilityManagerComponent::IsClient() const
{
	AActor* Owner = GetOwner();
	if (Owner->GetNetMode() == ENetMode::NM_Client
		|| Owner->Role < ENetRole::ROLE_Authority)
	{
		return true;
	}
	return false;
}

void UAMAbilityManagerComponent::BindOnAbilityReadDelegate(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput,
	EAMGroup InGroup, EAMSlot InSlot)
{

}
