// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"

#include "Abilities/GAAbilityBase.h"
#include "IAbilityFramework.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Attributes/GAAttributesBase.h"
#include "AFAbilityInterface.h"
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Effects/GAEffectExtension.h"
#include "Effects/GAEffectCue.h"
#include "AFCueManager.h"
#include "Effects/GABlueprintLibrary.h"
#include "Async.h"
#include "AFAbilityComponent.h"
#include "AFEffectsComponent.h"
#include "AFAbilityInterface.h"

DEFINE_STAT(STAT_ApplyEffect);
DEFINE_STAT(STAT_ModifyAttribute);



void FAFReplicatedAttributeItem::PreReplicatedRemove(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{
	FAFReplicatedAttributeContainer& ArraySerializer = const_cast<FAFReplicatedAttributeContainer&>(InArraySerializer);
	ArraySerializer.AttributeMap.Remove(AttributeTag);
}
void FAFReplicatedAttributeItem::PostReplicatedAdd(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{
	FAFReplicatedAttributeContainer& ArraySerializer = const_cast<FAFReplicatedAttributeContainer&>(InArraySerializer);
	UGAAttributesBase*& Attribute = ArraySerializer.AttributeMap.FindOrAdd(AttributeTag);
	Attribute = Attributes;
	InArraySerializer.OnAttributeReplicated(AttributeTag);
}
void FAFReplicatedAttributeItem::PostReplicatedChange(const struct FAFReplicatedAttributeContainer& InArraySerializer)
{

}
UGAAttributesBase* FAFReplicatedAttributeContainer::Add(const FGameplayTag InTag, UGAAttributesBase* InAttributes, class UAFAbilityComponent* InOuter)
{
	UGAAttributesBase* AttributesDup = DuplicateObject<UGAAttributesBase>(InAttributes, InOuter);
	FAFReplicatedAttributeItem Item;
	Item.AttributeTag = InTag;
	Item.Attributes = AttributesDup;
	Attributes.Add(Item);
	MarkItemDirty(Item);
	UGAAttributesBase*& Added = AttributeMap.FindOrAdd(InTag);
	Added = AttributesDup;
	return AttributesDup;
}
UAFAbilityComponent::UAFAbilityComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
	bIsAnyAbilityActive = false;
	bAutoActivate = true;
	bAutoRegister = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bRunOnAnyThread = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = true;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_DuringPhysics;
}

void UAFAbilityComponent::BroadcastAttributeChange(const FGAAttribute& InAttribute,
	const FAFAttributeChangedData& InData)
{
	FAFAttributeChangedDelegate* Delegate = AttributeChanged.Find(InAttribute);
	if (Delegate)
	{
		Delegate->Broadcast(InData);
	}
}

bool UAFAbilityComponent::GetShouldTick() const
{
	return true;
}

void UAFAbilityComponent::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	,FGAEffectProperty& InProperty)
{ 
	//OnAttributePreModifed.Broadcast(ModIn, 0);
	//Add log.
	if (!DefaultAttributes)
	{
		return;
	}
	float NewValue = DefaultAttributes->ModifyAttribute(ModIn, HandleIn, InProperty);
	FAFAttributeChangedData Data;
	FGAEffectContext& Context = InProperty.GetContext(HandleIn).GetRef();
	Data.Mod = ModIn;
	Data.Target = Context.Target;
	Data.Location = Context.HitResult.Location;
	OnAttributeModifed.Broadcast(Data);
	NotifyInstigatorTargetAttributeChanged(Data, Context);
};
void UAFAbilityComponent::NotifyInstigatorTargetAttributeChanged(const FAFAttributeChangedData& InData,
	const FGAEffectContext& InContext)
{
	InContext.InstigatorComp->OnTargetAttributeModifed.Broadcast(InData);
}
void UAFAbilityComponent::GetAttributeStructTest(FGAAttribute Name)
{
	DefaultAttributes->GetAttribute(Name);
}

void UAFAbilityComponent::OnRep_GameEffectContainer()
{
	float test = 0;
}
void UAFAbilityComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DefaultAttributes)
	{
		DefaultAttributes->Tick(DeltaTime);
	}
}

void UAFAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
	FAFEffectTimerManager::Get();
}
void UAFAbilityComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
}
void UAFAbilityComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	
}

void UAFAbilityComponent::OnAttributeModified(const FGAEffectMod& InMod, 
	const FGAEffectHandle& InHandle, UGAAttributesBase* InAttributeSet)
{
	
}

void UAFAbilityComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UAFAbilityComponent, DefaultAttributes);
	DOREPLIFETIME(UAFAbilityComponent, RepAttributes);

	DOREPLIFETIME(UAFAbilityComponent, ActiveCues);
	DOREPLIFETIME_CONDITION(UAFAbilityComponent, AbilityContainer, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UAFAbilityComponent, RepMontage, COND_SkipOwner);
}
void UAFAbilityComponent::OnRep_ActiveEffects()
{

}
void UAFAbilityComponent::OnRep_ActiveCues()
{

}

bool UAFAbilityComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	if (DefaultAttributes)
	{
		WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(DefaultAttributes), *Bunch, *RepFlags);
	}
	for (const FGASAbilityItem& Ability : AbilityContainer.AbilitiesItems)
	{
		//if (Set.InputOverride)
		//	WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASInputOverride*>(Set.InputOverride), *Bunch, *RepFlags);

		if (Ability.Ability)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAbilityBase*>(Ability.Ability), *Bunch, *RepFlags);
	}

	for (const FAFReplicatedAttributeItem& Attribute : RepAttributes.Attributes)
	{
		//if (Set.InputOverride)
		//	WroteSomething |= Channel->ReplicateSubobject(const_cast<UGASInputOverride*>(Set.InputOverride), *Bunch, *RepFlags);

		if (Attribute.Attributes)
			WroteSomething |= Channel->ReplicateSubobject(const_cast<UGAAttributesBase*>(Attribute.Attributes), *Bunch, *RepFlags);
	}

	return WroteSomething;
}
void UAFAbilityComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (DefaultAttributes && DefaultAttributes->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UGAAttributesBase*>(DefaultAttributes));
	}
}

void UAFAbilityComponent::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	if (IAFAbilityInterface* ABInterface = Cast<IAFAbilityInterface>(GetOwner()))
	{
		TagContainer = ABInterface->NativeGetEffectsComponent()->AppliedTags.AllTags;
	}
}
void UAFAbilityComponent::OnGameplayTaskActivated(UGameplayTask& Task)
{
	Super::OnGameplayTaskActivated(Task);

	if (UGAAbilityTask* ABTask = Cast<UGAAbilityTask>(&Task))
	{
		if (ABTask->IsReplicated())
		{
			if (SimulatedTasks.Contains(&Task) == false)
			{
				SimulatedTasks.Add(&Task);
				bIsNetDirty = true;
			}
		}
	}
}
const bool FGASAbilityItem::operator==(const FGameplayTag& AbilityTag) const
{
	return Ability->AbilityTag == AbilityTag;
}
void FGASAbilityItem::PreReplicatedRemove(const struct FGASAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		FGASAbilityContainer& InArraySerializerC = const_cast<FGASAbilityContainer&>(InArraySerializer);
		//remove attributes
		//UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = nullptr;
		FGameplayTag AbilityTag = Ability->AbilityTag;
		InArraySerializerC.RemoveAbilityFromAction(AbilityTag);
		InArraySerializerC.AbilitiesInputs.Remove(AbilityTag);
	}
}
void FGASAbilityItem::PostReplicatedAdd(const struct FGASAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		//should be safe, since we only modify the non replicated part of struct.
		FGASAbilityContainer& InArraySerializerC = const_cast<FGASAbilityContainer&>(InArraySerializer);
		Ability->AbilityComponent = InArraySerializer.AbilitiesComp.Get();
		if (InArraySerializer.AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(InArraySerializer.AbilitiesComp->GetOwner());
			Ability->POwner = POwner;
			Ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			Ability->OwnerCamera = nullptr;
		}
		Ability->InitAbility();
		Ability->Attributes = nullptr;
		UGAAttributesBase* attr = InArraySerializer.AbilitiesComp->RepAttributes.AttributeMap.FindRef(Ability->AbilityTag);
		Ability->Attributes = attr;
		InArraySerializerC.AbilitiesInputs.Add(Ability->AbilityTag, Ability); //.Add(Ability->AbilityTag, Ability);
		InArraySerializerC.AbilitiesComp->NotifyOnAbilityReady(Ability->AbilityTag);
	}
}
void FGASAbilityItem::PostReplicatedChange(const struct FGASAbilityContainer& InArraySerializer)
{

}
void FGASAbilityContainer::SetBlockedInput(const FGameplayTag& InActionName, bool bBlock)
{
	if (BlockedInput.Contains(InActionName))
	{
		BlockedInput[InActionName] = bBlock;
	}
	else
	{
		BlockedInput.Add(InActionName, bBlock);
	}
}
UGAAbilityBase* FGASAbilityContainer::AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn,
	AActor* InAvatar)
{
	if (AbilityIn && AbilitiesComp.IsValid())
	{
		
		UGAAbilityBase* ability = NewObject<UGAAbilityBase>(AbilitiesComp->GetOwner(), AbilityIn);
		ability->AbilityComponent = AbilitiesComp.Get();
		if (AbilitiesComp.IsValid())
		{
			APawn* POwner = Cast<APawn>(AbilitiesComp->GetOwner());
			ability->POwner = POwner;
			ability->PCOwner = Cast<APlayerController>(POwner->Controller);
			ability->OwnerCamera = nullptr;
			ability->AvatarActor = InAvatar;
		}
		ability->InitAbility();
		FGameplayTag Tag = ability->AbilityTag;

		AbilitiesInputs.Add(Tag, ability);
		FGASAbilityItem AbilityItem;
		MarkItemDirty(AbilityItem);
		AbilityItem.Ability = ability;
		AbilitiesItems.Add(AbilityItem);
		MarkArrayDirty();
		if (AbilitiesComp->GetNetMode() == ENetMode::NM_Standalone
			|| AbilitiesComp->GetOwnerRole() == ENetRole::ROLE_Authority)
		{
			AbilitiesComp->NotifyOnAbilityReady(ability->AbilityTag);
		}
	/*	if (ActionName.IsValid())
		{
			UInputComponent* InputComponent = AbilitiesComp->GetOwner()->FindComponentByClass<UInputComponent>();
			AbilitiesComp->BindAbilityToAction(InputComponent, ActionName, Tag);
		}*/
		return ability;
	}
	return nullptr;
}
void FGASAbilityContainer::RemoveAbility(const FGameplayTag& AbilityIn)
{
	int32 Index = AbilitiesItems.IndexOfByKey(AbilityIn);
	
	if (Index == INDEX_NONE)
		return;
	MarkItemDirty(AbilitiesItems[Index]);
	RemoveAbilityFromAction(AbilityIn);
	AbilitiesItems.RemoveAt(Index);
	MarkArrayDirty();
}
FGameplayTag FGASAbilityContainer::IsAbilityBoundToAction(const FGameplayTag& InInputTag)
{
	FGameplayTag Ability;
	FGameplayTag* AbilityTag = ActionToAbility.Find(InInputTag);
	if (AbilityTag)
	{
		Ability = *AbilityTag;
	}
	
	return Ability;
}
void FGASAbilityContainer::RemoveAbilityFromAction(const FGameplayTag& InAbilityTag)
{
	/*TArray<FGameplayTag> OutActions;
	AbilityToAction.RemoveAndCopyValue(InAbilityTag, OutActions);

	for(const FGameplayTag& Action : OutActions)
	{
		ActionToAbility.Remove(Action);
	}*/

	//AbilitiesInputs.Remove(InAbilityTag);
}
void FGASAbilityContainer::SetAbilityToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& InputTag : InInputTag)
	{
		if (ActionToAbility.Contains(InputTag))
		{
			FGameplayTag AbilityTag = ActionToAbility.FindRef(InputTag);
			UE_LOG(AbilityFramework, Log, TEXT("FGASAbilityContainer: Input %s is already abount to Ability %s"),
				*InputTag.ToString(),
				*AbilityTag.ToString()
			);

			//return;
		}

		FGameplayTag& AbilityTag = ActionToAbility.FindOrAdd(InputTag);
		AbilityTag = InAbilityTag;

		TArray<FGameplayTag>& ActionTag = AbilityToAction.FindOrAdd(InAbilityTag);
		ActionTag.Add(InputTag);
	}
	if (!AbilitiesComp.IsValid())
		return;
	if (AbilitiesComp->GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		AbilitiesComp->ClientNotifyAbilityInputReady(InAbilityTag);
	}
}

UGAAbilityBase* FGASAbilityContainer::GetAbility(FGameplayTag TagIn)
{
	UGAAbilityBase* retAbility = AbilitiesInputs.FindRef(TagIn);
	return retAbility;
}
void FGASAbilityContainer::HandleInputPressed(FGameplayTag ActionName, const FAFPredictionHandle& InPredictionHandle)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	FGameplayTag abilityTag = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(abilityTag);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		ability->OnNativeInputPressed(ActionName, InPredictionHandle);
	}
}
void FGASAbilityContainer::HandleInputReleased(FGameplayTag ActionName)
{
	if (BlockedInput.FindRef(ActionName))
	{
		return;
	}
	FGameplayTag abilityTag = ActionToAbility.FindRef(ActionName);
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(abilityTag);
	if (ability)
	{
		ability->OnNativeInputReleased(ActionName);
	}
}

void FGASAbilityContainer::TriggerAbylityByTag(FGameplayTag InTag)
{
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(InTag);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		FAFPredictionHandle PredHandle = FAFPredictionHandle::GenerateClientHandle(AbilitiesComp.Get());
		ability->OnNativeInputPressed(FGameplayTag(), PredHandle);
	}
}

void UAFAbilityComponent::InitializeComponent()
{
	Super::InitializeComponent();
	//PawnInterface = Cast<IIGIPawn>(GetOwner());
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	AbilityContainer.AbilitiesComp = this;
	//EffectTimerManager = MakeShareable(new FAFEffectTimerManager());
	
	//EffectTimerManager.InitThread();
	if (DefaultAttributes)
	{
		DefaultAttributes->InitializeAttributes(this);
		DefaultAttributes->InitializeAttributesFromTable();
	}
	ActiveCues.OwningComp = this;
	//ActiveCues.OwningComponent = this;
	AppliedTags.AddTagContainer(DefaultTags);
	InitializeInstancedAbilities();

	AActor* MyOwner = GetOwner();
	if (!MyOwner || !MyOwner->IsTemplate())
	{
		ULevel* ComponentLevel = (MyOwner ? MyOwner->GetLevel() : GetWorld()->PersistentLevel);
	}
}
void UAFAbilityComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	//EffectTimerManager.Deinitialize();
	//EffectTimerManager.Reset();
	//GameEffectContainer
}
void UAFAbilityComponent::BindInputs(class UInputComponent* InputComponent)
{
	for (const FGameplayTag& Tag : AbilityInputs)
	{
		BindAbilityToAction(InputComponent, Tag);
	}
}
void UAFAbilityComponent::SetBlockedInput(const FGameplayTag& InActionName, bool bBlock)
{
	AbilityContainer.SetBlockedInput(InActionName, bBlock);
}
void UAFAbilityComponent::BP_BindAbilityToAction(FGameplayTag ActionName)
{
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	check(InputComponent);

	BindAbilityToAction(InputComponent, ActionName);
}
void UAFAbilityComponent::BindAbilityToAction(UInputComponent* InputComponent, FGameplayTag ActionName)
{
	check(InputComponent);

	if (!InputComponent)
		return;

	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Pressed);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputPressed, ActionName);
		InputComponent->AddActionBinding(AB);
	}

	// Released event
	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Released);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputReleased, ActionName);
		InputComponent->AddActionBinding(AB);
	}
	SetBlockedInput(ActionName, false);
}
void UAFAbilityComponent::SetAbilityToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag
	, const FAFOnAbilityReady& InputDelegate)
{
	for (const FGameplayTag& Tag : InInputTag)
	{
		if (AbilityContainer.IsAbilityBoundToAction(Tag).IsValid())
		{
			RemoveAbilityFromAction(InAbilityTag, FGameplayTag());
		}
	}

	AbilityContainer.SetAbilityToAction(InAbilityTag, InInputTag);
	ENetRole role = GetOwnerRole();

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		&& role == ENetRole::ROLE_AutonomousProxy)
	{
		if (InputDelegate.IsBound())
		{
			AddOnAbilityInputReadyDelegate(InAbilityTag, InputDelegate);
		}
		ServerSetAbilityToAction(InAbilityTag, InInputTag);
	}
}

FGameplayTag UAFAbilityComponent::IsAbilityBoundToAction(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& Tag : InInputTag)
	{
		return AbilityContainer.IsAbilityBoundToAction(Tag);
		break;
	}
	return FGameplayTag();
}
void UAFAbilityComponent::RemoveAbilityFromAction(const FGameplayTag& InAbilityTag, const FGameplayTag& InInputTag)
{
	AbilityContainer.RemoveAbilityFromAction(InAbilityTag);
}
void UAFAbilityComponent::ServerSetAbilityToAction_Implementation(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		SetAbilityToAction(InAbilityTag, InInputTag, FAFOnAbilityReady());
	}
}
bool UAFAbilityComponent::ServerSetAbilityToAction_Validate(const FGameplayTag& InAbilityTag, const TArray<FGameplayTag>& InInputTag)
{
	return true;
}

void UAFAbilityComponent::ClientNotifyAbilityInputReady_Implementation(FGameplayTag AbilityTag)
{
	NotifyOnAbilityInputReady(AbilityTag);
}

void UAFAbilityComponent::SetAbilitiesToActions(const TArray<FAFAbilityActionSet>& InAbilitiesActions
	, const TArray<FAFOnAbilityReady>& InputDelegate)
{
	for (const FAFAbilityActionSet& Set : InAbilitiesActions)
	{
		for (const FGameplayTag& Tag : Set.AbilityInputs)
		{
			if (AbilityContainer.IsAbilityBoundToAction(Tag).IsValid())
			{
				RemoveAbilityFromAction(Set.AbilityTag, FGameplayTag());
			}
		}
	}
	for (const FAFAbilityActionSet& Set : InAbilitiesActions)
	{
		AbilityContainer.SetAbilityToAction(Set.AbilityTag, Set.AbilityInputs);
	}
	ENetRole role = GetOwnerRole();

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		&& role == ENetRole::ROLE_AutonomousProxy)
	{
		for (int32 Idx = 0; Idx < InAbilitiesActions.Num(); Idx++)
		{
			if (InputDelegate[Idx].IsBound())
			{
				AddOnAbilityInputReadyDelegate(InAbilitiesActions[Idx].AbilityTag, InputDelegate[Idx]);
			}
		}
		
		ServerSetAbilitiesToActions(InAbilitiesActions);
	}
}

void UAFAbilityComponent::ServerSetAbilitiesToActions_Implementation(const TArray<FAFAbilityActionSet>& InAbilitiesActions)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		for (const FAFAbilityActionSet& Set : InAbilitiesActions)
		{
			SetAbilityToAction(Set.AbilityTag, Set.AbilityInputs, FAFOnAbilityReady());
		}
	}
}
bool UAFAbilityComponent::ServerSetAbilitiesToActions_Validate(const TArray<FAFAbilityActionSet>& InAbilitiesActions)
{
	return true;
}

void UAFAbilityComponent::BP_InputPressed(FGameplayTag ActionName)
{
	NativeInputPressed(ActionName);
}
void UAFAbilityComponent::NativeInputPressed(FGameplayTag ActionName)
{
	FAFPredictionHandle PredHandle;
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		PredHandle = FAFPredictionHandle::GenerateClientHandle(this);
		AbilityContainer.HandleInputPressed(ActionName, PredHandle);
	}
	ServerNativeInputPressed(ActionName, PredHandle);
		
}

void UAFAbilityComponent::ServerNativeInputPressed_Implementation(FGameplayTag ActionName, FAFPredictionHandle InPredictionHandle)
{
	AbilityContainer.HandleInputPressed(ActionName, InPredictionHandle);
	//NativeInputPressed(ActionName);
}
bool UAFAbilityComponent::ServerNativeInputPressed_Validate(FGameplayTag ActionName, FAFPredictionHandle InPredictionHandle)
{
	return true;
}

void UAFAbilityComponent::BP_InputReleased(FGameplayTag ActionName)
{
	NativeInputReleased(ActionName);
}

void UAFAbilityComponent::NativeInputReleased(FGameplayTag ActionName)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		AbilityContainer.HandleInputReleased(ActionName);
	}
	ServerNativeInputReleased(ActionName);
}

void UAFAbilityComponent::ServerNativeInputReleased_Implementation(FGameplayTag ActionName)
{
	AbilityContainer.HandleInputReleased(ActionName);
}
bool UAFAbilityComponent::ServerNativeInputReleased_Validate(FGameplayTag ActionName)
{
	return true;
}
void UAFAbilityComponent::BP_AddAbilityFromTag(FGameplayTag InAbilityTag,
	AActor* InAvatar, TArray<FGameplayTag> InInputTag)
{
	NativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
}
void UAFAbilityComponent::NativeAddAbilityFromTag(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	FGameplayTag AlreadyBound = IsAbilityBoundToAction(InAbilityTag, InInputTag);
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
		if (AlreadyBound.IsValid())
		{
			for (const FGameplayTag& Input : InInputTag)
			{
				AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
			}
		}
	}
	else
	{
		if (AlreadyBound.IsValid())
		{
			AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
		}
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

			TArray<FAssetData> AssetData;
			FARFilter Filter;
			Filter.TagsAndValues.Add("AbilityTagSearch", InAbilityTag.ToString());
			AssetRegistryModule.Get().GetAssets(Filter, AssetData);
			FPrimaryAssetId PrimaryAssetId = FPrimaryAssetId(FPrimaryAssetType("Ability"), AssetData[0].AssetName);
			FPrimaryAssetTypeInfo Info;
			if (Manager->GetPrimaryAssetTypeInfo(PrimaryAssetId.PrimaryAssetType, Info))
			{
				FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFAbilityComponent::OnFinishedLoad, InAbilityTag, PrimaryAssetId, InAvatar);

				Manager->LoadPrimaryAsset(PrimaryAssetId,
					TArray<FName>(),
					del);
			}
		}
	}
	
}

void UAFAbilityComponent::ServerNativeAddAbilityFromTag_Implementation(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	NativeAddAbilityFromTag(InAbilityTag, InAvatar, InInputTag);
}

bool UAFAbilityComponent::ServerNativeAddAbilityFromTag_Validate(FGameplayTag InAbilityTag,
	AActor* InAvatar, const TArray<FGameplayTag>& InInputTag)
{
	return true;
}
void UAFAbilityComponent::BP_RemoveAbility(FGameplayTag TagIn)
{

}
void UAFAbilityComponent::NativeRemoveAbility(const FGameplayTag& InAbilityTag)
{
	AbilityContainer.RemoveAbility(InAbilityTag);
}
void UAFAbilityComponent::ServerNativeRemoveAbility_Implementation(FGameplayTag InAbilityTag)
{
	NativeRemoveAbility(InAbilityTag);
}

bool UAFAbilityComponent::ServerNativeRemoveAbility_Validate(FGameplayTag InAbilityTag)
{
	return true;
}
void UAFAbilityComponent::OnFinishedLoad(FGameplayTag InAbilityTag,
	FPrimaryAssetId InPrimaryAssetId, AActor* InAvatar)
{
	if (AbilityContainer.AbilityExists(InAbilityTag))
	{
		return;
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}
	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		UObject* loaded = Manager->GetPrimaryAssetObject(InPrimaryAssetId);
		TSubclassOf<UGAAbilityBase> cls = Cast<UClass>(loaded);
		if (cls)
		{
			InstanceAbility(cls, InAvatar);
		}

		{
			Manager->UnloadPrimaryAsset(InPrimaryAssetId);
		}
	}
}


UGAAbilityBase* UAFAbilityComponent::BP_GetAbilityByTag(FGameplayTag TagIn)
{
	UGAAbilityBase* retVal = AbilityContainer.GetAbility(TagIn);
	return retVal;
}
UGAAbilityBase* UAFAbilityComponent::InstanceAbility(TSubclassOf<class UGAAbilityBase> AbilityClass,
	AActor* InAvatar)
{
	if (AbilityClass)
	{
		UGAAbilityBase* ability = AbilityContainer.AddAbility(AbilityClass, InAvatar);
		AbilityContainer.MarkArrayDirty();
		return ability;
	}
	return nullptr;
}

void UAFAbilityComponent::OnRep_InstancedAbilities()
{
}
void UAFAbilityComponent::NotifyOnAbilityAdded(const FGameplayTag& InAbilityTag)
{
	OnAbilityAdded.Broadcast(InAbilityTag);
	
}
void UAFAbilityComponent::InitializeInstancedAbilities()
{
}

void UAFAbilityComponent::OnRep_PlayMontage()
{
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(RepMontage.CurrentMontage);
		if (RepMontage.SectionName != NAME_None)
		{
			AnimInst->Montage_JumpToSection(RepMontage.SectionName, RepMontage.CurrentMontage);
		}
		UE_LOG(AbilityFramework, Log, TEXT("OnRep_PlayMontage MontageName: %s SectionNAme: %s ForceRep: %s"), *RepMontage.CurrentMontage->GetName(), *RepMontage.SectionName.ToString(), *FString::FormatAsNumber(RepMontage.ForceRep)); 
	}
}

void UAFAbilityComponent::PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed)
{
	//if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		//Probabaly want to do something different here for client non authority montage plays.
		//return;
	}
	ACharacter* MyChar = Cast<ACharacter>(GetOwner());
	if (MyChar)
	{
		UAnimInstance* AnimInst = MyChar->GetMesh()->GetAnimInstance();
		AnimInst->Montage_Play(MontageIn, Speed);
		if (SectionName != NAME_None)
		{
			//AnimInst->Montage_JumpToSection(SectionName, MontageIn);
		}

		UE_LOG(AbilityFramework, Log, TEXT("PlayMontage MontageName: %s SectionNAme: %s Where: %s"), *MontageIn->GetName(), *SectionName.ToString(), (GetOwnerRole() < ENetRole::ROLE_Authority ? TEXT("Client") : TEXT("Server")));
		RepMontage.SectionName = SectionName;
		RepMontage.CurrentMontage = MontageIn;
		RepMontage.ForceRep++;
	}
}
void UAFAbilityComponent::MulticastPlayMontage_Implementation(UAnimMontage* MontageIn, FName SectionName, float Speed = 1)
{

}