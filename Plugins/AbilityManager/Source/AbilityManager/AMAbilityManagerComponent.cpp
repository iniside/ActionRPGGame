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
	// ...
}


// Called when the game starts
void UAMAbilityManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}
void UAMAbilityManagerComponent::InitializeComponent()
{
	AbilitySet.SetNum(MaxGroups);
	AbilityTagsSet.SetNum(MaxGroups);
	for(int32 Idx = 0; Idx < MaxGroups; Idx++)
	{
		AbilitySet[Idx].SetNum(1);
		AbilityTagsSet[Idx].SetNum(1);
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
	return AbilitySet.Num() >= MaxGroups ? AbilitySet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)].Get() : nullptr;
}
void UAMAbilityManagerComponent::SetAbility(EAMGroup InGroup, EAMSlot InSlot, UGAAbilityBase* InAbility)
{
	if (AbilitySet.Num() < MaxGroups)
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
void UAMAbilityManagerComponent::BP_EquipAbility(const FGameplayTag& InAbilityTag, EAMGroup InGroup, EAMSlot InSlot)
{
	NativeEquipAbility(InAbilityTag, InGroup, InSlot);
}
FGameplayTag UAMAbilityManagerComponent::GetAbilityTag(EAMGroup InGroup, EAMSlot InSlot)
{
	return AbilityTagsSet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)];
}
void UAMAbilityManagerComponent::SetAbilityTag(EAMGroup InGroup, EAMSlot InSlot, FGameplayTag InAbilityTag)
{
	AbilityTagsSet[AMEnumToInt<EAMGroup>(InGroup)][AMEnumToInt<EAMSlot>(InSlot)] = InAbilityTag;
}
void UAMAbilityManagerComponent::NativeEquipAbility(const FGameplayTag& InAbilityTag, EAMGroup InGroup, EAMSlot InSlot, AActor* InAvatar)
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
	TArray<FGameplayTag> IAbilityInput = GetInputTag(InGroup, InSlot);
	FAFOnAbilityReady del = FAFOnAbilityReady::CreateUObject(this, &UAMAbilityManagerComponent::OnAbilityReady, InAbilityTag,
		IAbilityInput, InGroup, InSlot);

	AbilityComp->AddOnAbilityReadyDelegate(InAbilityTag, del);
	AbilityComp->NativeAddAbilityFromTag(InAbilityTag, InAvatar, IAbilityInput);// , /*Input*/ ShootInput);
}
void UAMAbilityManagerComponent::OnAbilityReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput, 
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
	}
	else
	{
		SetAbility(InGroup, InSlot, Ability);
		SetAbilityTag(InGroup, InSlot, InAbilityTag);
		SetInputTag(InGroup, InSlot, InAbilityInput);
		AbilityComp->SetAbilityToAction(InAbilityTag, InAbilityInput, FAFOnAbilityReady());
		ExecuteAbilityReadyEvent(InAbilityTag);
	}
}

void UAMAbilityManagerComponent::OnAbilityInputReady(FGameplayTag InAbilityTag, TArray<FGameplayTag> InAbilityInput,
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
	ExecuteAbilityReadyEvent(InAbilityTag);
}

void UAMAbilityManagerComponent::NextGroup()
{
	ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
		CurrentIndex++;
		if(CurrentIndex > MaxGroups)
		{
			ActiveGroup = EAMGroup::Group001;
		}
		else
		{
			ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
		}
		
		if (ActiveGroup < AMIntToEnum<EAMGroup>(MaxGroups))
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
	if (ActiveGroup >= EAMGroup::Group001)
	{
	}
	else
	{
		ActiveGroup = EAMGroup::Group001;
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
	if (CurrentIndex > MaxGroups)
	{
		ActiveGroup = EAMGroup::Group001;
	}
	else
	{
		ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	}
	if (ActiveGroup < AMIntToEnum<EAMGroup>(MaxGroups))
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
		ClientNextGroup(AMEnumToInt<EAMGroup>(ActiveGroup));
	}
}
bool UAMAbilityManagerComponent::ServerNextGroup_Validate(int32 WeaponIndex)
{
	return true;
}
void UAMAbilityManagerComponent::ClientNextGroup_Implementation(int32 WeaponIndex)
{
	ActiveGroup = AMIntToEnum<EAMGroup>(WeaponIndex);
}

void UAMAbilityManagerComponent::ServerPreviousGroup_Implementation(int32 WeaponIndex)
{
	int32 CurrentIndex = AMEnumToInt<EAMGroup>(ActiveGroup);
	CurrentIndex--;
	ActiveGroup = AMIntToEnum<EAMGroup>(CurrentIndex);
	if (ActiveGroup >= EAMGroup::Group001)
	{
	}
	else
	{
		ActiveGroup = EAMGroup::Group001;
	}
	if (ActiveGroup != AMIntToEnum<EAMGroup>(WeaponIndex))
	{
		ClientPreviousGroup(AMEnumToInt<EAMGroup>(ActiveGroup));
	}
}
bool UAMAbilityManagerComponent::ServerPreviousGroup_Validate(int32 WeaponIndex)
{
	return true;
}
void UAMAbilityManagerComponent::ClientPreviousGroup_Implementation(int32 WeaponIndex)
{
	ActiveGroup = AMIntToEnum<EAMGroup>(WeaponIndex);
}

void UAMAbilityManagerComponent::SelectGroup(EAMGroup InGroup)
{
	
}