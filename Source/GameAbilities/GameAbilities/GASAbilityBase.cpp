// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#include "GameAbilities.h"
#include "Tasks/GASAbilityTask.h"
#include "GAGlobalTypes.h"
#include "GAEffectGlobalTypes.h"
#include "GASAbilitiesComponent.h"
#include "GAAttributeComponent.h"
#include "GAGameEffect.h"
#include "AbilityCues/GASCueActor.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"

#include "GASAbilityBase.h"

UGASAbilityBase::UGASAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsAbilityExecuting = false;
	bIsOnCooldown = false;
	bReplicate = true;
	bIsNameStable = false;
}

void UGASAbilityBase::InitAbility()
{
	if (OwningComp.IsValid())
	{
		World = OwningComp->GetWorld();
	}
	if (POwner && POwner->GetNetMode() != ENetMode::NM_Standalone)
	{
		InitAbilityCounter++;
	}
	else
	{
		OnRep_InitAbility();
	}
}
void UGASAbilityBase::OnRep_InitAbility()
{
	if (!ActorCue)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Instigator = POwner;
		SpawnParams.Owner = POwner;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		//can be null.
		UWorld* world = GetWorld();
		if (world)
		{
			ActorCue = world->SpawnActor<AGASCueActor>(ActorCueClass, POwner->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
			if (ActorCue)
			{
				ActorCue->OwningAbility = this;
			}
		}
	}
}
void UGASAbilityBase::OnNativeInputPressed()
{
	//Check for cooldown and don't all inputs if on cooldown. Because.
	if (CanUseAbility())
	{
		UE_LOG(GameAbilities, Log, TEXT("OnNativeInputPressed in ability %s"), *GetName());
		OnInputPressed();
	}
	else
	{
		UE_LOG(GameAbilities, Log, TEXT("OnNativeInputPressed in ability %s is on Cooldown."), *GetName());
	}
}

void UGASAbilityBase::OnNativeInputReleased()
{
	if (CanReleaseAbility())
	{
		UE_LOG(GameAbilities, Log, TEXT("OnNativeInputReleased in ability %s"), *GetName());
		OnInputReleased();
	}
}

void UGASAbilityBase::OnAbilityExecutedNative()
{
	UE_LOG(GameAbilities, Log, TEXT("Begin Executing Ability: %s"), *GetName());

	if (OnConfirmCastingEndedDelegate.IsBound())
	{
		OnConfirmCastingEndedDelegate.Broadcast();
	}
	ApplyActivationEffect();
	bIsAbilityExecuting = true;
	//OnAbilityExecuted();
}

void UGASAbilityBase::ExecuteAbility()
{
	if (CheckCooldown() || CheckExecuting())
	{
		return;
	}
	AbilityComponent->ExecutingAbility = this;
	OnAbilityExecutedNative();
}

void UGASAbilityBase::OnCooldownEffectExpired()
{
	UE_LOG(GameAbilities, Log, TEXT("Cooldown expired In Ability: %s"), *GetName());
	bIsOnCooldown = false;
	CooldownEffectExpiredCounter++;
	if (CooldownHandle.IsValid())
	{
		CooldownHandle.GetContextRef().InstigatorComp->RemoveEffect(CooldownHandle);
	}
}
void UGASAbilityBase::OnActivationEffectExpired()
{
	//OnAbilityExecutedNative();
	//this works under assumption that current state == activation state.
	UE_LOG(GameAbilities, Log, TEXT("Ability Activation Effect Expired In Ability: %s"), *GetName());
	bIsAbilityExecuting = false;
	AbilityComponent->ExecutingAbility = nullptr;
	AbilityActivatedCounter++;
	if (ActivationEffectHandle.IsValid())
	{
		ActivationEffectHandle.GetContextRef().InstigatorComp->RemoveEffect(ActivationEffectHandle);
	}
	OnAbilityExecuted();
}
void UGASAbilityBase::OnActivationEffectRemoved()
{
	//OnAbilityExecutedNative();
	//this works under assumption that current state == activation state.
	UE_LOG(GameAbilities, Log, TEXT("Ability Activation Effect Expired In Ability: %s"), *GetName());
	bIsAbilityExecuting = false;
	AbilityComponent->ExecutingAbility = nullptr;
	OnStopAbility();
	//AbilityActivatedCounter++;
}
void UGASAbilityBase::OnActivationEffectPeriod()
{
	UE_LOG(GameAbilities, Log, TEXT("Ability Activation Effect Period In Ability: %s"), *GetName());
	AbilityPeriodCounter++;
	OnAbilityPeriod();
}
void UGASAbilityBase::StopAbility()
{
	if (ActivationEffectHandle.IsValid())
	{
		ActivationEffectHandle.GetContextRef().InstigatorComp->RemoveEffect(ActivationEffectHandle);
	}
	AbilityComponent->ExecutingAbility = nullptr;
	bIsAbilityExecuting = false;
}

void UGASAbilityBase::OnNativeStopAbility()
{
	OnStopAbility();
}

void UGASAbilityBase::OnAbilityCancelNative()
{
	OnAbilityCancel();
}

void UGASAbilityBase::FinishExecution()
{
	UE_LOG(GameAbilities, Log, TEXT("FinishExecution in ability %s"), *GetName());
	bIsAbilityExecuting = false;
	NativeFinishExecution();
}
void UGASAbilityBase::NativeFinishExecution()
{
	UE_LOG(GameAbilities, Log, TEXT("NativeFinishExecution in ability %s"), *GetName());
	bIsAbilityExecuting = false;
	AbilityComponent->ExecutingAbility = nullptr;
	if (ActivationEffectHandle.IsValid())
	{
		ActivationEffectHandle.GetContextRef().InstigatorComp->RemoveEffect(ActivationEffectHandle);
	}
	//remove effect.
	OnFinishExecution();
}

bool UGASAbilityBase::IsWaitingForConfirm()
{
	if (OnConfirmDelegate.IsBound())
		return true;
	else
		return false;
}
void UGASAbilityBase::ConfirmAbility()
{
	if (OnConfirmDelegate.IsBound())
		OnConfirmDelegate.Broadcast();
	OnConfirmDelegate.Clear();
}

bool UGASAbilityBase::ApplyCooldownEffect()
{
	if (!CooldownEffect.Spec)
		return false;

	if (CooldownEffect.Spec->Duration > 0)
	{
		UE_LOG(GameAbilities, Log, TEXT("Set cooldown effect in Ability: %s"), *GetName());
		CooldownHandle = ApplyEffectToActor(
			CooldownEffect, CooldownHandle,
			POwner, POwner, this);

		if (CooldownHandle.IsValid())
		{
			LastCooldownTime = GetWorld()->GetTimeSeconds();
			TSharedPtr<FGAGameEffect> Effect = CooldownHandle.GetEffectPtr();
			if (!Effect->OnEffectExpired.IsBound())
			{
				UE_LOG(GameAbilities, Log, TEXT("Bind effect cooldown in Ability: %s"), *GetName());
				Effect->OnEffectExpired.BindUObject(this, &UGASAbilityBase::OnCooldownEffectExpired);
			}
		}
		bIsOnCooldown = true;
		CooldownStartedCounter++;
		return true;
	}
	return false;
}
bool UGASAbilityBase::ApplyActivationEffect()
{
	if (!ActivationEffect.Spec)
		return false;

	if (ActivationEffect.Spec->Duration > 0)
	{
		UE_LOG(GameAbilities, Log, TEXT("Set expiration effect in Ability: %s"), *GetName());
		ActivationEffectHandle = ApplyEffectToActor(
			ActivationEffect, ActivationEffectHandle,
			POwner, POwner, this);

		if (ActivationEffectHandle.IsValid())
		{
			bIsAbilityExecuting = true;
			LastActivationTime = GetWorld()->GetTimeSeconds();
			TSharedPtr<FGAGameEffect> Effect = ActivationEffectHandle.GetEffectPtr();
			if (!Effect->OnEffectExpired.IsBound())
			{
				UE_LOG(GameAbilities, Log, TEXT("Bind effect expiration in Ability: %s"), *GetName());
				Effect->OnEffectExpired.BindUObject(this, &UGASAbilityBase::OnActivationEffectExpired);
			}
			if (!Effect->OnEffectRemoved.IsBound())
			{
				UE_LOG(GameAbilities, Log, TEXT("Bind effect removed in Ability: %s"), *GetName());
				Effect->OnEffectRemoved.BindUObject(this, &UGASAbilityBase::OnActivationEffectRemoved);
			}
			if (ActivationEffect.Spec->Period > 0)
			{
				if (!Effect->OnEffectPeriod.IsBound())
				{
					UE_LOG(GameAbilities, Log, TEXT("Bind effect period in Ability: %s"), *GetName());
					Effect->OnEffectPeriod.BindUObject(this, &UGASAbilityBase::OnActivationEffectPeriod);
				}
			}
			AbilityActivationStartedCounter++;
			return true;
		}
	}
	else
	{
		OnActivationEffectExpired();
	}
	return false;
}

bool UGASAbilityBase::CanUseAbility()
{
	bool CanUse = true;
	if(!bIsOnCooldown)
		UE_LOG(GameAbilities, Log, TEXT("CanUseAbility bIsOnCooldown is true"));
	
	if (!AbilityComponent->ExecutingAbility)
		UE_LOG(GameAbilities, Log, TEXT("CanUseAbility AbilityComponent->ExecutingAbility is true"));

	CanUse = !bIsOnCooldown && !AbilityComponent->ExecutingAbility;
	return CanUse;
}

bool UGASAbilityBase::CanReleaseAbility()
{
	bool bCanUse = true;
	if (AbilityComponent->ExecutingAbility == this)
	{
		bCanUse = true;
	}
	if (bIsOnCooldown)
	{
		bCanUse = false;
		UE_LOG(GameAbilities, Log, TEXT("CanReleaseAbility can't release ability is on cooldown"));
	}
	return bCanUse;
}

float UGASAbilityBase::GetCooldownTime() const
{
	return GetWorld()->GetTimeSeconds() - LastCooldownTime;
}
float UGASAbilityBase::BP_GetCooldownTime() const
{
	return GetCooldownTime();
}
float UGASAbilityBase::GetActivationTime() const
{
	return GetWorld()->GetTimeSeconds() - LastActivationTime;
}
float UGASAbilityBase::BP_GetActivationTime() const
{
	return GetActivationTime();
}

void UGASAbilityBase::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	if (UGASAbilityTask* task = Cast<UGASAbilityTask>(&Task))
	{
		task->Ability = this;
	}
}
UGameplayTasksComponent* UGASAbilityBase::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return OwningComp.Get();
}
/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnGameplayTaskActivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Started; %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Add(&Task);
}
/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
void UGASAbilityBase::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(GameAbilities, Log, TEXT("Task Removed: %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Remove(&Task);
}
AActor* UGASAbilityBase::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return POwner;
}
AActor* UGASAbilityBase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return AvatarActor;
}

class UGAAttributesBase* UGASAbilityBase::GetAttributes()
{
	return Attributes;
}
float UGASAbilityBase::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return NativeGetAttributeValue(AttributeIn);
}
float UGASAbilityBase::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return Attributes->GetCurrentAttributeValue(AttributeIn);
}

FGAGameEffectHandle UGASAbilityBase::ApplyEffectToActor(const FGAEffectSpec& SpecIn,
	FGAGameEffectHandle HandleIn, class AActor* Target, class APawn* Instigator,
	UObject* Causer)
{
	if (!SpecIn.Spec)
	{
		return FGAGameEffectHandle();
	}
	FGAEffectContext Context = MakeActorContext(Target, Instigator, Causer);
	if (!Context.IsValid())
	{
		//if the handle is valid (valid pointer to effect and id)
		//we want to preseve it and just set bad context.
		if (HandleIn.IsValid())
		{
			HandleIn.SetContext(Context);
			Context.InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffect(), HandleIn);
			return HandleIn;
		}
		else
		{
			return FGAGameEffectHandle();
		}
	}
	if (HandleIn.IsValid())
	{
		HandleIn.SetContext(Context);
	}
	else
	{
		FGAGameEffect* effect = new FGAGameEffect(SpecIn.Spec, Context);
		AddTagsToEffect(effect);
		//FGAGameEffectHandle& HandleCon = const_cast<FGAGameEffectHandle&>(HandleIn);
		HandleIn = FGAGameEffectHandle::GenerateHandle(effect);
		effect->Handle = &HandleIn;
	}
	Context.InstigatorComp->ApplyEffectToTarget(HandleIn.GetEffect(), HandleIn);
	return HandleIn;
}

void UGASAbilityBase::RemoveEffectFromActor(FGAGameEffectHandle& HandleIn, class AActor* TargetIn)
{
	IIGAAttributes* TargetAttr = Cast<IIGAAttributes>(TargetIn);
	if (!TargetAttr)
		return;

	UGAAttributeComponent* TargetComp = TargetAttr->GetAttributeComponent();
	TargetComp->RemoveEffect(HandleIn);
}

FGAEffectContext UGASAbilityBase::MakeActorContext(class AActor* Target, class APawn* Instigator, UObject* Causer)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target);
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
	{
		return FGAEffectContext();
	}

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();
	FVector location = Target->GetActorLocation();
	FGAEffectContext Context(location, Target, Causer,
		Instigator, targetComp, instiComp);
	return Context;
}
FGAEffectContext UGASAbilityBase::MakeHitContext(const FHitResult& Target, class APawn* Instigator, UObject* Causer)
{
	IIGAAttributes* targetAttr = Cast<IIGAAttributes>(Target.GetActor());
	IIGAAttributes* instiAttr = Cast<IIGAAttributes>(Instigator);
	if (!targetAttr || !instiAttr)
	{
		return FGAEffectContext();
	}

	UGAAttributeComponent* targetComp = targetAttr->GetAttributeComponent();
	UGAAttributeComponent* instiComp = instiAttr->GetAttributeComponent();

	FGAEffectContext Context(Target.Location, Target.GetActor(), Causer,
		Instigator, targetComp, instiComp);
	return Context;
}
void UGASAbilityBase::AddTagsToEffect(FGAGameEffect* EffectIn)
{
	if (EffectIn)
	{
		EffectIn->AddOwnedTags(EffectIn->GameEffect->OwnedTags);
		EffectIn->AddApplyTags(EffectIn->GameEffect->ApplyTags);
	}
}

bool UGASAbilityBase::ApplyAttributeCost()
{
	return false;
}

bool UGASAbilityBase::CheckCooldown()
{
	return bIsOnCooldown; //temp
}
bool UGASAbilityBase::CheckExecuting()
{
	return bIsAbilityExecuting; //temp
}
void UGASAbilityBase::BP_ApplyCooldown()
{
	ApplyCooldownEffect();
}

float UGASAbilityBase::GetCurrentActivationTime()
{
	if (ActivationEffectHandle.IsValid())
	{
		return ActivationEffectHandle.GetEffectPtr()->GetCurrentActivationTime();
	}
	return 0;
}

float UGASAbilityBase::CalculateAnimationSpeed(UAnimMontage* MontageIn)
{
	float ActivationTime = MontageIn->GetPlayLength();
	if (ActivationEffectHandle.IsValid())
	{
		ActivationTime = ActivationEffectHandle.GetEffectPtr()->GetActivationTime();
	}
	float Duration = MontageIn->GetPlayLength();
	
	float PlaySpeed = Duration / ActivationTime;
	return PlaySpeed;
}


bool UGASAbilityBase::IsNameStableForNetworking() const
{
	return bIsNameStable;
}

void UGASAbilityBase::SetNetAddressable()
{
	bIsNameStable = true;
}

class UWorld* UGASAbilityBase::GetWorld() const
{
	return World;
}
void UGASAbilityBase::PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed)
{
	AbilityComponent->PlayMontage(MontageIn, SectionName, Speed);
}

void UGASAbilityBase::ActivateActorCue(FVector Location)
{
	if (ActorCue)
	{

		ActorCue->SetActorLocation(Location);
		ActorCue->SetActorHiddenInGame(false);
		ActorCue->OnActivated();
	}
}

void UGASAbilityBase::MulticastActivateActorCue_Implementation(FVector Location)
{

}

//replication
void UGASAbilityBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGASAbilityBase, POwner);
	DOREPLIFETIME(UGASAbilityBase, PCOwner);
	DOREPLIFETIME(UGASAbilityBase, AICOwner);
	//probabaly should be SKIP_Owner, and I'm not really sure if Multicast wouldn't be better.
	DOREPLIFETIME(UGASAbilityBase, CooldownStartedCounter);
	DOREPLIFETIME(UGASAbilityBase, CooldownEffectExpiredCounter);
	DOREPLIFETIME(UGASAbilityBase, AbilityActivatedCounter);
	DOREPLIFETIME(UGASAbilityBase, AbilityActivationStartedCounter);
	DOREPLIFETIME(UGASAbilityBase, AbilityPeriodCounter);
	DOREPLIFETIME(UGASAbilityBase, InitAbilityCounter);
	DOREPLIFETIME(UGASAbilityBase, AbilityHits);
	//DOREPLIFETIME(UGASAbilitiesComponent, RepMontage);
}
/*
	Do some yet undertemined client stuff.
	Call events ?
*/
void UGASAbilityBase::OnRep_CooldownStarted()
{

}
void UGASAbilityBase::OnRep_CooldownExpired()
{

}
void UGASAbilityBase::OnRep_AbilityActivationStarted()
{

}
void UGASAbilityBase::OnRep_AbilityActivated()
{

}
void UGASAbilityBase::OnRep_AbilityPeriod()
{

}
void UGASAbilityBase::OnRep_AbilityHits()
{

}