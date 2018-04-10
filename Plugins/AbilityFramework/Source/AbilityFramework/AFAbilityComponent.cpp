// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"

#include "Abilities/GAAbilityBase.h"
#include "Abilities/Tasks/GAAbilityTask.h"
#include "IAbilityFramework.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Attributes/GAAttributesBase.h"
#include "AFAbilityInterface.h"
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"

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
	//add default replication (PropertyRep) that attribute changed.
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
	for (const FAFAbilityItem& Ability : AbilityContainer.AbilitiesItems)
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
void UAFAbilityComponent::SetAbilityToAction(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr, const TArray<FGameplayTag>& InInputTag
	, const FAFOnAbilityReady& InputDelegate)
{
	AbilityContainer.SetAbilityToAction(InAbilityPtr, InInputTag);
	ENetRole role = GetOwnerRole();

	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		&& role == ENetRole::ROLE_AutonomousProxy)
	{
		if (InputDelegate.IsBound())
		{
			AddOnAbilityInputReadyDelegate(InAbilityPtr, InputDelegate);
		}
		ServerSetAbilityToAction(InAbilityPtr, InInputTag);
	}
}

TSoftClassPtr<UGAAbilityBase> UAFAbilityComponent::IsAbilityBoundToAction(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr, const TArray<FGameplayTag>& InInputTag)
{
	for (const FGameplayTag& Tag : InInputTag)
	{
		return AbilityContainer.IsAbilityBoundToAction(Tag);
		break;
	}
	return TSoftClassPtr<UGAAbilityBase>();
}

void UAFAbilityComponent::ServerSetAbilityToAction_Implementation(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr, const TArray<FGameplayTag>& InInputTag)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_DedicatedServer)
	{
		SetAbilityToAction(InAbilityPtr, InInputTag, FAFOnAbilityReady());
	}
}
bool UAFAbilityComponent::ServerSetAbilityToAction_Validate(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr, const TArray<FGameplayTag>& InInputTag)
{
	return true;
}

void UAFAbilityComponent::ClientNotifyAbilityInputReady_Implementation(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr)
{
	NotifyOnAbilityInputReady(InAbilityPtr);
	UGAAbilityBase* Ability = AbilityContainer.TagToAbility.FindRef(InAbilityPtr);
	if (Ability)
	{
		Ability->OnAbilityInputReady();
	}
}

void UAFAbilityComponent::SetAbilitiesToActions(const TArray<FAFAbilityActionSet>& InAbilitiesActions
	, const TArray<FAFOnAbilityReady>& InputDelegate)
{
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

void UAFAbilityComponent::BP_AddAbility(TSoftClassPtr<UGAAbilityBase> InAbility,
	TArray<FGameplayTag> InInputTag)
{
	NativeAddAbility(InAbility, InInputTag);
}

void UAFAbilityComponent::NativeAddAbility(TSoftClassPtr<UGAAbilityBase> InAbility,
	const TArray<FGameplayTag>& InInputTag)
{
	//FGameplayTag AlreadyBound = IsAbilityBoundToAction(InAbilityTag, InInputTag);
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeAddAbility(InAbility, InInputTag);
		/*if (AlreadyBound.IsValid())
		{
			for (const FGameplayTag& Input : InInputTag)
			{
				AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
			}
		}*/
	}
	else
	{
		/*if (AlreadyBound.IsValid())
		{
			AbilityContainer.RemoveAbilityFromAction(AlreadyBound);
		}*/
		if (UAssetManager* Manager = UAssetManager::GetIfValid())
		{
			FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
			FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
			{
				FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFAbilityComponent::OnFinishedLoad, InAbility);

				StreamManager.RequestAsyncLoad(InAbility.ToSoftObjectPath()
					, del);
			}
		}
	}
}


void UAFAbilityComponent::ServerNativeAddAbility_Implementation(const TSoftClassPtr<UGAAbilityBase>& InAbility,
	const TArray<FGameplayTag>& InInputTag)
{
	NativeAddAbility(InAbility, InInputTag);
}

bool UAFAbilityComponent::ServerNativeAddAbility_Validate(const TSoftClassPtr<UGAAbilityBase>& InAbility,
	const TArray<FGameplayTag>& InInputTag)
{
	return true;
}
void UAFAbilityComponent::OnFinishedLoad(TSoftClassPtr<UGAAbilityBase> InAbility)
{
	if (AbilityContainer.AbilityExists(InAbility))
	{
		return;
	}
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}
	
	
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();

	TSubclassOf<UGAAbilityBase> cls = InAbility.Get();
	if (cls)
	{
		InstanceAbility(cls, InAbility);
	}

	{
		Manager.Unload(InAbility.ToSoftObjectPath());
	}
}

void UAFAbilityComponent::BP_RemoveAbility(TSoftClassPtr<UGAAbilityBase> TagIn)
{

}
void UAFAbilityComponent::NativeRemoveAbility(TSoftClassPtr<UGAAbilityBase> InAbilityTag)
{
	AbilityContainer.RemoveAbility(InAbilityTag);
}
void UAFAbilityComponent::ServerNativeRemoveAbility_Implementation(const TSoftClassPtr<UGAAbilityBase>& InAbilityTag)
{
	NativeRemoveAbility(InAbilityTag);
}

bool UAFAbilityComponent::ServerNativeRemoveAbility_Validate(const TSoftClassPtr<UGAAbilityBase>& InAbilityTag)
{
	return true;
}

UGAAbilityBase* UAFAbilityComponent::BP_GetAbilityByTag(TSoftClassPtr<UGAAbilityBase> TagIn)
{
	UGAAbilityBase* retVal = AbilityContainer.GetAbility(TagIn);
	return retVal;
}
UGAAbilityBase* UAFAbilityComponent::InstanceAbility(TSubclassOf<class UGAAbilityBase> AbilityClass
	, TSoftClassPtr<class UGAAbilityBase> InClassPtr)
{
	if (AbilityClass)
	{
		UGAAbilityBase* ability = AbilityContainer.AddAbility(AbilityClass, InClassPtr);
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