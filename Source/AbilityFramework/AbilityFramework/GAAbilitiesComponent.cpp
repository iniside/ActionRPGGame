// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"

#include "Abilities/GAAbilityBase.h"

#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GAGlobals.h"
#include "Abilities/GAAbilitySet.h"
#include "IGIPawn.h"
#include "Attributes/GAAttributesBase.h"
#include "IGAAbilities.h"
#include "Effects/GAEffectExecution.h"
#include "Effects/GAGameEffect.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"
#include "Effects/GAEffectExtension.h"
#include "GAAbilitiesComponent.h"



DEFINE_STAT(STAT_ApplyEffect);
DEFINE_STAT(STAT_ModifyAttribute);

UGAAbilitiesComponent::UGAAbilitiesComponent(const FObjectInitializer& ObjectInitializer)
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
bool UGAAbilitiesComponent::GetShouldTick() const
{
	return true;
}

void UGAAbilitiesComponent::ModifyAttribute(FGAEffectMod& ModIn, const FGAEffectHandle& HandleIn
	,FGAEffectProperty& InProperty)
{ 
	UGAAbilitiesComponent* blaBla = this;
	//send message to self.
	//MessageEndpoint->Send<FGAModifyAttributeValueMessage>(Message, GetMessageAddress());
	float NewValue = DefaultAttributes->ModifyAttribute(ModIn, HandleIn, InProperty);
	OnAttributeChanged.Broadcast(NewValue);
};

void UGAAbilitiesComponent::GetAttributeStructTest(FGAAttribute Name)
{
	DefaultAttributes->GetAttribute(Name);
}

void UGAAbilitiesComponent::OnRep_GameEffectContainer()
{
	float test = 0;
}
void UGAAbilitiesComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	UActorComponent::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (DefaultAttributes)
	{
		DefaultAttributes->Tick(DeltaTime);
	}
}
void UGAAbilitiesComponent::Update()
{

}
void UGAAbilitiesComponent::BeginPlay()
{
	Super::BeginPlay();
}
void UGAAbilitiesComponent::DestroyComponent(bool bPromoteChildren)
{
	Super::DestroyComponent(bPromoteChildren);
	//PrimaryComponentTick.SetTickFunctionEnable(false);
	//PrimaryComponentTick.bCanEverTick = false;
	//PrimaryComponentTick.bRunOnAnyThread = true;
	//PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	
	//PrimaryComponentTick.UnRegisterTickFunction();
}

FGAEffectHandle UGAAbilitiesComponent::ApplyEffectToSelf(FGAEffect* EffectIn
	,FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{
	//OnEffectApplyToSelf.Broadcast(HandleIn, HandleIn.GetEffectPtr()->OwnedTags);
	GameEffectContainer.ApplyEffect(EffectIn, InProperty);
	//FGAEffectCueParams CueParams;
	//CueParams.HitResult = EffectIn.Context.HitResult;
	//OnEffectApplied.Broadcast(HandleIn, HandleIn.GetEffectPtr()->OwnedTags);
	return FGAEffectHandle();
}
FGAEffectHandle UGAAbilitiesComponent::ApplyEffectToTarget(FGAEffect* EffectIn
	, FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{
	//FGAEffectCueParams CueParams;
	//CueParams.HitResult = EffectIn.Context.HitResult;
	//execute cue from effect regardless if we have target object or not.
	InContext.TargetComp->ApplyEffectToSelf(EffectIn, InProperty, InContext);
	//MulticastApplyEffectCue(HandleIn, CueParams);

//	if (EffectIn.IsValid() && EffectIn.Context.TargetComp.IsValid())
//	{
		//OnEffectApplyToTarget.Broadcast(HandleIn, HandleIn.GetEffectPtr()->OwnedTags);
	//	return EffectIn.Context.TargetComp->ApplyEffectToSelf(EffectIn, HandleIn);
//	}
	return FGAEffectHandle();
}

FGAEffectHandle UGAAbilitiesComponent::MakeGameEffect(TSubclassOf<class UGAGameEffectSpec> SpecIn,
	const FGAEffectContext& ContextIn)
{
	FGAEffect* effect = new FGAEffect(SpecIn.GetDefaultObject(), ContextIn);
	if (effect)
	{
		effect->OwnedTags.AppendTags(effect->GameEffect->OwnedTags);
		effect->ApplyTags.AppendTags(effect->GameEffect->ApplyTags);
	}
	FGAEffectHandle handle = FGAEffectHandle::GenerateHandle(effect);
	effect->Handle = handle;
	return handle;
}
void UGAAbilitiesComponent::OnAttributeModified(const FGAEffectMod& InMod, 
	const FGAEffectHandle& InHandle, UGAAttributesBase* InAttributeSet)
{

}
void UGAAbilitiesComponent::ExecuteEffect(FGAEffectHandle HandleIn, FGAEffectProperty InProperty)
{
	/*
	this patth will give effects chance to do any replicated events, like applying cues.
	WE do not make any replication at the ApplyEffect because some effect might want to apply cues
	on periods on expiration etc, and all those will go trouch ExecuteEffect path.
	*/
	OnEffectExecuted.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Effect %s executed"), *HandleIn.GetEffectSpec()->GetName());
	FGAEffect& Effect = HandleIn.GetEffectRef();
	FGAEffectMod Mod = Effect.GetAttributeModifier();

	//execute period regardless if this periodic effect ? Or maybe change name OnEffectExecuted ?
	
	Effect.OnExecuted();
	MulticastExecuteEffectCue(HandleIn);
	HandleIn.ExecuteEffect(HandleIn, Mod, HandleIn.GetContextRef(), InProperty);

	//GameEffectContainer.ExecuteEffect(HandleIn, HandleIn.GetEffectRef());
}
void UGAAbilitiesComponent::ExpireEffect(FGAEffectHandle HandleIn, FGAEffectProperty InProperty)
{
	//call effect internal delegate:
	//HandleIn.GetEffectPtr()->OnExpired();
	//InternalRemoveEffect(HandleIn);
	//OnEffectExpired.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
	GameEffectContainer.RemoveEffectByHandle(HandleIn, InProperty);
}
void UGAAbilitiesComponent::RemoveEffect(const FGAEffectProperty& InProperty)
{
	InternalRemoveEffect(InProperty);
	//OnEffectRemoved.Broadcast(HandleIn, HandleIn.GetEffectSpec()->OwnedTags);
}
void UGAAbilitiesComponent::InternalRemoveEffect(const FGAEffectProperty& InProperty)
{
	//FTimerManager& timer = GetWorld()->GetTimerManager();
	//timer.ClearTimer(HandleIn.GetEffectRef().PeriodTimerHandle);
	//timer.ClearTimer(HandleIn.GetEffectRef().DurationTimerHandle);
	UE_LOG(GameAttributesEffects, Log, TEXT("UGAAbilitiesComponent:: Reset Timers and Remove Effect"));

	//MulticastRemoveEffectCue(HandleIn);
	//periodic effects do not apply duration based modifiers to attributes.
	//yet in anycase.
	GameEffectContainer.RemoveEffect(InProperty);
}

void UGAAbilitiesComponent::ApplyInstacnedEffectToSelf(class UGAEffectExtension* EffectIn)
{
	GameEffectContainer.ApplyEffectInstance(EffectIn);
}
void UGAAbilitiesComponent::ApplyInstancedToTarget(class UGAEffectExtension* EffectIn)
{
	//FGAEffectCueParams CueParams;
	//CueParams.HitResult = EffectIn->Context.HitResult;
	//execute cue from effect regardless if we have target object or not.
	//MulticastApplyEffectCue(HandleIn, EffectIn->Cue, CueParams);
	EffectIn->Context.TargetComp->ApplyInstacnedEffectToSelf(EffectIn);
}

void UGAAbilitiesComponent::RemoveInstancedFromSelf(class UGAEffectExtension* EffectIn)
{

}
TArray<FGAEffectUIData> UGAAbilitiesComponent::GetEffectUIData()
{
	TArray<FGAEffectUIData> dataReturn;
	return dataReturn;
}

int32 UGAAbilitiesComponent::GetEffectUIIndex()
{
	return 1;
}
FGAEffectUIData UGAAbilitiesComponent::GetEffectUIDataByIndex(int32 IndexIn)
{
	FGAEffectUIData data;
	return data;
}

void UGAAbilitiesComponent::MulticastApplyEffectCue_Implementation(FGAEffectHandle EffectHandle, FGAEffectCueParams CueParams)
{
	float test = 0;
	ActiveCues.AddCue(EffectHandle, CueParams);
}

void UGAAbilitiesComponent::MulticastExecuteEffectCue_Implementation(FGAEffectHandle EffectHandle)
{
	//	ActiveCues.ExecuteCue(EffectHandle);
}

void UGAAbilitiesComponent::MulticastRemoveEffectCue_Implementation(FGAEffectHandle EffectHandle)
{
	//	ActiveCues.RemoveCue(EffectHandle);
}

void UGAAbilitiesComponent::MulticastUpdateDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}
void UGAAbilitiesComponent::MulticastUpdatePeriodCue_Implementation(FGAEffectHandle EffectHandle, float NewPeriodIn)
{

}
void UGAAbilitiesComponent::MulticastUpdateTimersCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn, float NewPeriodIn)
{

}
void UGAAbilitiesComponent::MulticastExtendDurationCue_Implementation(FGAEffectHandle EffectHandle, float NewDurationIn)
{

}

void UGAAbilitiesComponent::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly replicate it to everyone
	//to allow prediction for UI.
	DOREPLIFETIME(UGAAbilitiesComponent, DefaultAttributes);
	DOREPLIFETIME(UGAAbilitiesComponent, ModifiedAttribute);
	DOREPLIFETIME(UGAAbilitiesComponent, GameEffectContainer);

	DOREPLIFETIME(UGAAbilitiesComponent, ActiveCues);

	DOREPLIFETIME(UGAAbilitiesComponent, AbilityContainer);
	DOREPLIFETIME_CONDITION(UGAAbilitiesComponent, RepMontage, COND_SkipOwner);
}
void UGAAbilitiesComponent::OnRep_ActiveEffects()
{

}
void UGAAbilitiesComponent::OnRep_ActiveCues()
{

}

void UGAAbilitiesComponent::OnRep_AttributeChanged()
{
	for (FGAModifiedAttribute& attr : ModifiedAttribute.Mods)
	{
		attr.Causer->OnAttributeModifed.Broadcast(attr);
	}
}
bool UGAAbilitiesComponent::ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags)
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
	return WroteSomething;
}
void UGAAbilitiesComponent::GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs)
{
	if (DefaultAttributes && DefaultAttributes->IsNameStableForNetworking())
	{
		Objs.Add(const_cast<UGAAttributesBase*>(DefaultAttributes));
	}
}


FGAGenericDelegate& UGAAbilitiesComponent::GetTagEvent(FGameplayTag TagIn)
{
	FGAGenericDelegate& Delegate = GenericTagEvents.FindChecked(TagIn);
	return Delegate;
}
void UGAAbilitiesComponent::NativeTriggerTagEvent(FGameplayTag TagIn)
{
	FGAGenericDelegate* Delegate = GenericTagEvents.Find(TagIn);
	if (Delegate)
	{
		if (Delegate->IsBound())
		{
			Delegate->Broadcast();
		}
	}
}
void UGAAbilitiesComponent::BP_TriggerTagEvent(FGameplayTag TagIn)
{
	NativeTriggerTagEvent(TagIn);
}
void UGAAbilitiesComponent::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = AppliedTags.AllTags;
}
bool UGAAbilitiesComponent::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return AppliedTags.HasTag(TagToCheck);
}

bool UGAAbilitiesComponent::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AppliedTags.HasAll(TagContainer);
}

bool UGAAbilitiesComponent::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return AppliedTags.HasAny(TagContainer);
}


void FGASAbilityItem::PreReplicatedRemove(const struct FGASAbilityContainer& InArraySerializer)
{

}
void FGASAbilityItem::PostReplicatedAdd(const struct FGASAbilityContainer& InArraySerializer)
{
	if (InArraySerializer.AbilitiesComp.IsValid())
	{
		//should be safe, since we only modify the non replicated part of struct.
		FGASAbilityContainer& InArraySerializerC = const_cast<FGASAbilityContainer&>(InArraySerializer);
		InArraySerializerC.AbilitiesInputs.Add(Ability->AbilityTag, Ability); //.Add(Ability->AbilityTag, Ability);
	}
}
void FGASAbilityItem::PostReplicatedChange(const struct FGASAbilityContainer& InArraySerializer)
{

}

UGAAbilityBase* FGASAbilityContainer::AddAbility(TSubclassOf<class UGAAbilityBase> AbilityIn, FGameplayTag ActionName)
{
	if (AbilityIn && AbilitiesComp.IsValid())
	{
		UGAAbilityBase* ability = NewObject<UGAAbilityBase>(AbilitiesComp->GetOwner(), AbilityIn);
		ability->AbilityComponent = AbilitiesComp.Get();
		if (AbilitiesComp->PawnInterface)
		{
			ability->POwner = AbilitiesComp->PawnInterface->GetGamePawn();
			ability->PCOwner = AbilitiesComp->PawnInterface->GetGamePlayerController();
			ability->OwnerCamera = AbilitiesComp->PawnInterface->GetPawnCamera();
			//ability->AIOwner = PawnInterface->GetGameController();
		}
		ability->InitAbility();
		FGameplayTag Tag = ability->AbilityTag;

		AbilitiesInputs.Add(Tag, ability);
		FGASAbilityItem AbilityItem;
		AbilityItem.Ability = ability;
		AbilitiesItems.Add(AbilityItem);
		if (ActionName.IsValid())
		{
			UInputComponent* InputComponent = AbilitiesComp->GetOwner()->FindComponentByClass<UInputComponent>();
			AbilitiesComp->BindAbilityToAction(InputComponent, ActionName, Tag);
		}
		return ability;
	}
	return nullptr;
}
UGAAbilityBase* FGASAbilityContainer::GetAbility(FGameplayTag TagIn)
{
	return nullptr;
}
void FGASAbilityContainer::HandleInputPressed(FGameplayTag TagIn, FGameplayTag ActionName)
{
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(TagIn);
	if (ability)
	{
		if (ability->IsWaitingForConfirm())
		{
			ability->ConfirmAbility();
			return;
		}
		ability->OnNativeInputPressed(ActionName);
	}
}
void FGASAbilityContainer::HandleInputReleased(FGameplayTag TagIn, FGameplayTag ActionName)
{
	UGAAbilityBase* ability = AbilitiesInputs.FindRef(TagIn);
	if (ability)
	{
		ability->OnNativeInputReleased(ActionName);
	}
}

bool UGAAbilitiesComponent::CanActivateAbility()
{
	//if there are no activate abilities, we can activate the selected one.
	//this should be configarable ie. how many abilities can be activated at the same time ?
	//if (!bIsAnyAbilityActive && !ExecutingAbility)
	//{
	//	return true;
	//}
	if (!ExecutingAbility)
	{
		return true;
	}
	return false;
}
void UGAAbilitiesComponent::InitializeComponent()
{
	Super::InitializeComponent();
	PawnInterface = Cast<IIGIPawn>(GetOwner());
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	AbilityContainer.AbilitiesComp = this;

	if (DefaultAttributes)
	{
		DefaultAttributes->InitializeAttributes(this);
	}
	GameEffectContainer.OwningComponent = this;
	ActiveCues.OwningComp = this;
	//ActiveCues.OwningComponent = this;
	AppliedTags.AddTagContainer(DefaultTags);
	//TestRunnable = new FAsyncUObjectRunnable(this);
	//TEstAsyncUObject = NewObject<UAsyncUObject>(this, UAsyncUObject::StaticClass(), TEXT("AwesomeObject"), RF_StrongRefOnFrame | RF_Standalone);
	//TEstAsyncUObject->AddToRoot();
	//RouterThread = FRunnableThread::Create(TEstAsyncUObject, TEXT("AsyncUObject.Test"), 128 * 1024, TPri_Normal, FPlatformAffinity::GetPoolThreadMask());
	InitializeInstancedAbilities();
}
void UGAAbilitiesComponent::UninitializeComponent()
{
	Super::UninitializeComponent();
	//GameEffectContainer
}
void UGAAbilitiesComponent::BP_BindAbilityToAction(FGameplayTag ActionName, FGameplayTag AbilityTag)
{
	UInputComponent* InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	BindAbilityToAction(InputComponent, ActionName, AbilityTag);
}
void UGAAbilitiesComponent::BindAbilityToAction(UInputComponent* InputComponent, FGameplayTag ActionName, FGameplayTag AbilityTag)
{
	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Pressed);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UGAAbilitiesComponent::NativeInputPressed, AbilityTag, ActionName);
		InputComponent->AddActionBinding(AB);
	}

	// Released event
	{
		FInputActionBinding AB(ActionName.GetTagName(), IE_Released);
		AB.ActionDelegate.GetDelegateForManualSet().BindUObject(this, &UGAAbilitiesComponent::NativeInputReleased, AbilityTag, ActionName);
		InputComponent->AddActionBinding(AB);
	}
}
void UGAAbilitiesComponent::BP_InputPressed(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	NativeInputPressed(AbilityTag, ActionName);
}
void UGAAbilitiesComponent::NativeInputPressed(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeInputPressed(AbilityTag, ActionName);
	}
	else
	{
		UE_LOG(AbilityFramework, Log, TEXT("UGAAbilitiesComponent::NativeInputPressed: %s"), *AbilityTag.GetTagName().ToString());
		AbilityContainer.HandleInputPressed(AbilityTag, ActionName);
	}
}

void UGAAbilitiesComponent::ServerNativeInputPressed_Implementation(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	NativeInputPressed(AbilityTag, FGameplayTag());
}
bool UGAAbilitiesComponent::ServerNativeInputPressed_Validate(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	return true;
}

void UGAAbilitiesComponent::BP_InputReleased(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	NativeInputReleased(AbilityTag, ActionName);
}

void UGAAbilitiesComponent::NativeInputReleased(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerNativeInputReleased(AbilityTag, ActionName);
	}
	else
	{
		UE_LOG(AbilityFramework, Log, TEXT("UGAAbilitiesComponent::NativeInputReleased: %s"), *AbilityTag.GetTagName().ToString());

		AbilityContainer.HandleInputReleased(AbilityTag, ActionName);
	}
}

void UGAAbilitiesComponent::ServerNativeInputReleased_Implementation(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	NativeInputReleased(AbilityTag, FGameplayTag());
}
bool UGAAbilitiesComponent::ServerNativeInputReleased_Validate(FGameplayTag AbilityTag, FGameplayTag ActionName)
{
	return true;
}

void UGAAbilitiesComponent::BP_AddAbility(TSubclassOf<class UGAAbilityBase> AbilityClass, FGameplayTag ActionName)
{
	//AddAbilityToActiveList(AbilityClass);
	InstanceAbility(AbilityClass, ActionName);
}

void UGAAbilitiesComponent::BP_RemoveAbility(FGameplayTag TagIn)
{
	
}
UGAAbilityBase* UGAAbilitiesComponent::BP_GetAbilityByTag(FGameplayTag TagIn)
{
	return AbilityContainer.GetAbility(TagIn);
}
UGAAbilityBase* UGAAbilitiesComponent::InstanceAbility(TSubclassOf<class UGAAbilityBase> AbilityClass, FGameplayTag ActionName)
{
	if (AbilityClass)
	{
		UGAAbilityBase* ability = AbilityContainer.AddAbility(AbilityClass, ActionName);
		return ability;
	}
	return nullptr;
}

void UGAAbilitiesComponent::NativeAddAbilitiesFromSet(TSubclassOf<class UGAAbilitySet> AbilitySet)
{
	//do not let add abilities on non authority.
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
		return;

	UGAAbilitySet* Set = AbilitySet.GetDefaultObject();
	int32 Index = 0;
	for (const FGASAbilitySetItem& Item : Set->AbilitySet.Abilities)
	{
		InstanceAbility(Item.AbilityClass, Item.Binding);
		Index++;
	}
}

void UGAAbilitiesComponent::BP_AddAbilitiesFromSet(TSubclassOf<class UGAAbilitySet> AbilitySet)
{
	NativeAddAbilitiesFromSet(AbilitySet);
}

void UGAAbilitiesComponent::OnRep_InstancedAbilities()
{
}

void UGAAbilitiesComponent::InitializeInstancedAbilities()
{
}

void UGAAbilitiesComponent::OnRep_PlayMontage()
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
void UGAAbilitiesComponent::PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
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
			AnimInst->Montage_JumpToSection(SectionName, MontageIn);
		}
		UE_LOG(AbilityFramework, Log, TEXT("PlayMontage MontageName: %s SectionNAme: %s Where: %s"), *MontageIn->GetName(), *SectionName.ToString(), (GetOwnerRole() < ENetRole::ROLE_Authority ? TEXT("Client") : TEXT("Server")));
		RepMontage.SectionName = SectionName;
		RepMontage.CurrentMontage = MontageIn;
		RepMontage.ForceRep++;
	}
}