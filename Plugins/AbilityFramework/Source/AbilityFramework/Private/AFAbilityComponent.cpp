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
#include "Effects/AFCueActor.h"
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
	InArraySerializer.OnAttributeReplicated(AttributeTag, Attributes);
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
	OnAttributeReplicated(InTag, AttributesDup);
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

void UAFAbilityComponent::ModifyAttribute(FGAEffectMod& ModIn
	, const FGAEffectHandle& HandleIn
	, FGAEffectProperty& InProperty
	, const FGAEffectContext& InContext)
{ 
	//OnAttributePreModifed.Broadcast(ModIn, 0);
	//Add log.
	if (!DefaultAttributes)
	{
		return;
	}
	float NewValue = DefaultAttributes->ModifyAttribute(ModIn, HandleIn, InProperty, InContext);
	FAFAttributeChangedData Data;
	Data.Mod = ModIn;
	Data.Target = InContext.Target;
	Data.Location = InContext.HitResult.Location;
	OnAttributeModifed.Broadcast(Data);
	InContext.InstigatorComp->NotifyInstigatorTargetAttributeChanged(Data, InContext);
	//add default replication (PropertyRep) that attribute changed.
};
void UAFAbilityComponent::NotifyInstigatorTargetAttributeChanged(const FAFAttributeChangedData& InData,
	const FGAEffectContext& InContext)
{
	ENetMode NM = GetOwner()->GetNetMode();

	switch (NM)
	{
	case NM_Standalone:
		ClientNotifyAttributeModifier(InData);
		break;
	case NM_DedicatedServer:
		ServerOnTargetAttributeModifed.Broadcast(InData);
		ClientNotifyAttributeModifier(InData);
		break;
	case NM_ListenServer:
		ServerOnTargetAttributeModifed.Broadcast(InData);
		ClientNotifyAttributeModifier(InData);
		break;
	case NM_Client:
		ClientNotifyAttributeModifier(InData);
		break;
	case NM_MAX:
		break;
	default:
		break;
	}
}
void UAFAbilityComponent::ClientNotifyAttributeModifier_Implementation(const FAFAttributeChangedData& InData)
{
	OnTargetAttributeModifed.Broadcast(InData);
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
	//FAFEffectTimerManager::Get();
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
	for (const FAFAbilitySpec& Ability : AbilityContainer.ActivatableAbilities)
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
void UAFAbilityComponent::BindInputs(class UInputComponent* InputComponent, FString BindEnum)
{
	UEnum* bindEnum = FindObject<UEnum>(ANY_PACKAGE, *BindEnum);
	if (!bindEnum)
		return;

	int32 num = bindEnum->NumEnums();
	
	for (uint8 Idx = 0; Idx < num; Idx++)
	{
		FString Name = bindEnum->GetNameStringByIndex(Idx);
		{
			FInputActionBinding AB(FName(*Name), IE_Pressed);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputPressed, Idx);
			InputComponent->AddActionBinding(AB);
		}

		// Released event
		{
			FInputActionBinding AB(FName(*Name), IE_Released);
			AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UAFAbilityComponent::NativeInputReleased, Idx);
			InputComponent->AddActionBinding(AB);
		}
	}
}

void UAFAbilityComponent::SetBlockedInput(const FGameplayTag& InActionName, bool bBlock)
{
}



TSoftClassPtr<UGAAbilityBase> UAFAbilityComponent::IsAbilityBoundToAction(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr, const TArray<FGameplayTag>& InInputTag)
{
	return TSoftClassPtr<UGAAbilityBase>();
}


void UAFAbilityComponent::BindAbilityToInputIDs(const FAFAbilitySpecHandle Handle, TArray<uint8> InputIDs)
{
	AbilityContainer.BindAbilityToInputIDs(Handle, InputIDs);
	ENetRole role = GetOwnerRole();
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client
		&& role == ENetRole::ROLE_AutonomousProxy)
	{
		/*for (int32 Idx = 0; Idx < InAbilitiesActions.Num(); Idx++)
		{
			if (InputDelegate[Idx].IsBound())
			{
				AddOnAbilityInputReadyDelegate(InAbilitiesActions[Idx].AbilityTag, InputDelegate[Idx]);
			}
		}*/

		ServerBindAbilityToInputIDs(Handle, InputIDs);
	}
}

void UAFAbilityComponent::ServerBindAbilityToInputIDs_Implementation(const FAFAbilitySpecHandle Handle, const TArray<uint8>& InputIDs)
{
	AbilityContainer.BindAbilityToInputIDs(Handle, InputIDs);
}
bool UAFAbilityComponent::ServerBindAbilityToInputIDs_Validate(const FAFAbilitySpecHandle Handle, const TArray<uint8>& InputIDs)
{
	return true;
}

void UAFAbilityComponent::NativeInputPressed(uint8 InputID)
{
	FAFPredictionHandle PredHandle;
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		PredHandle = FAFPredictionHandle::GenerateClientHandle(this);
		AbilityContainer.HandleInputPressed(InputID, PredHandle);
	}
	ServerNativeInputPressed(InputID, PredHandle);
}

void UAFAbilityComponent::ServerNativeInputPressed_Implementation(uint8 InputID, FAFPredictionHandle InPredictionHandle)
{
	AbilityContainer.HandleInputPressed(InputID, InPredictionHandle);
}
bool UAFAbilityComponent::ServerNativeInputPressed_Validate(uint8 InputID, FAFPredictionHandle InPredictionHandle)
{
	return true;
}

void UAFAbilityComponent::NativeInputReleased(uint8 InputID)
{
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		AbilityContainer.HandleInputReleased(InputID);
	}
	ServerNativeInputReleased(InputID);
}

void UAFAbilityComponent::ServerNativeInputReleased_Implementation(uint8 InputID)
{
	AbilityContainer.HandleInputReleased(InputID);
}
bool UAFAbilityComponent::ServerNativeInputReleased_Validate(uint8 InputID)
{
	return true;
}

void UAFAbilityComponent::BP_AddAbility(TSoftClassPtr<UGAAbilityBase> InAbility,
	TArray<FGameplayTag> InInputTag)
{
}

void UAFAbilityComponent::NativeAddAbility(TSoftClassPtr<UGAAbilityBase> InAbility, const FAFAbilitySpecHandle ClientHandle)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeAddAbility(InAbility.ToSoftObjectPath(), ClientHandle);
	}
}

void UAFAbilityComponent::NativeAddAbilityFromObject(UGAAbilityBase* InAbility, const FAFAbilitySpecHandle ClientHandle)
{
	
}

void UAFAbilityComponent::ServerNativeAddAbility_Implementation(const FSoftObjectPath& InAbility, const FAFAbilitySpecHandle& ClientHandle)
{
	FAFAbilitySpecHandle ServerHandle = FAFAbilitySpecHandle::GenerateHandle();

	if (UAssetManager* Manager = UAssetManager::GetIfValid())
	{
		FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
		IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();
		FStreamableManager& StreamManager = UAssetManager::GetStreamableManager();
		{
			TSoftClassPtr<UGAAbilityBase> Ab(InAbility);
			FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &UAFAbilityComponent::OnFinishedLoad, Ab, ServerHandle, ClientHandle);

			StreamManager.RequestAsyncLoad(InAbility
				, del);
		}
	}
}

bool UAFAbilityComponent::ServerNativeAddAbility_Validate(const FSoftObjectPath& InAbility, const FAFAbilitySpecHandle& ClientHandle)
{
	return true;
}
void UAFAbilityComponent::OnFinishedLoad(TSoftClassPtr<UGAAbilityBase> InAbility, const FAFAbilitySpecHandle Handle, const FAFAbilitySpecHandle ClientHandle)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}
	FStreamableManager& Manager = UAssetManager::GetStreamableManager();

	TSubclassOf<UGAAbilityBase> cls = InAbility.Get();
	if (cls)
	{
		UGAAbilityBase* ability = AbilityContainer.AddAbility(cls, Handle, ClientHandle);
		AbilityContainer.MarkArrayDirty();
	}

	{
		Manager.Unload(InAbility.ToSoftObjectPath());
	}
}

void UAFAbilityComponent::BP_RemoveAbility(TSoftClassPtr<UGAAbilityBase> TagIn)
{

}
void UAFAbilityComponent::NativeRemoveAbility(const FAFAbilitySpecHandle InHandle)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeRemoveAbility(InHandle);
	}
	AbilityContainer.RemoveAbility(InHandle);
}
void UAFAbilityComponent::ServerNativeRemoveAbility_Implementation(const FAFAbilitySpecHandle& InHandle)
{
	AbilityContainer.RemoveAbility(InHandle);
}

bool UAFAbilityComponent::ServerNativeRemoveAbility_Validate(const FAFAbilitySpecHandle& InHandle)
{
	return true;
}

UGAAbilityBase* UAFAbilityComponent::BP_GetAbilityByHandle(FAFAbilitySpecHandle TagIn)
{
	return AbilityContainer.GetAbility(TagIn);
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
void UAFAbilityComponent::ClientNotifyAbilityInputReady_Implementation(const TSoftClassPtr<UGAAbilityBase>& InAbilityPtr)
{

}