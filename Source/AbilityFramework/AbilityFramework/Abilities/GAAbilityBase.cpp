// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#include "../AbilityFramework.h"
#include "Tasks/GAAbilityTask.h"
#include "../Effects/GAGameEffect.h"
#include "../GAGlobalTypes.h"
#include "../Effects/GAEffectGlobalTypes.h"
#include "../GAAbilitiesComponent.h"

#include "../AbilityCues/GACueActor.h"
#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "../AbilityCues/GAAbilityCue.h"
#include "../Effects/GABlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GAAbilityBase.h"

void FGAAbilityTick::ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	if (Target && !Target->IsPendingKillOrUnreachable())
	{
		FScopeCycleCounterUObject ActorScope(Target);
		Target->TickAbility(DeltaTime, TickType, *this);
	}
}

FString FGAAbilityTick::DiagnosticMessage()
{
	return Target->GetFullName() + TEXT("[TickAction]");
}

UGAAbilityBase::UGAAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicate = true;
	bIsNameStable = false;

	TickFunction.TickGroup = TG_PrePhysics;
	// Default to no tick function, but if we set 'never ticks' to false (so there is a tick function) it is enabled by default
	TickFunction.bCanEverTick = true;
	TickFunction.bStartWithTickEnabled = true;
	TickFunction.bAllowTickOnDedicatedServer = true;
	TickFunction.bRunOnAnyThread = true;

	TickFunction.SetTickFunctionEnable(true);
}
void UGAAbilityBase::TickAbility(float DeltaSeconds, ELevelTick TickType, FGAAbilityTick& ThisTickFunction)
{

}

void UGAAbilityBase::InitAbility()
{
	if (AbilityComponent)
	{
		World = AbilityComponent->GetWorld();
	}
	if (POwner && POwner->GetNetMode() != ENetMode::NM_Standalone)
	{
		InitAbilityCounter++;
	}
	else
	{
		OnRep_InitAbility();
	}
	if (!AbilityComponent)
	{
		AbilityComponent = GetAbilityComp();
		if (AbilityComponent)
		{
			AbilityComponent->AddAddtionalAttributes(AbilityTag, Attributes);
		}
	}
	if (Attributes)
	{
		Attributes->InitializeAttributes(GetAbilityComp());
	}
	
	if (!OwnerCamera)
	{
		OwnerCamera = POwner->FindComponentByClass<UCameraComponent>();
	}
	TickFunction.Target = this;
	TickFunction.RegisterTickFunction(AbilityComponent->GetWorld()->GetCurrentLevel());
	TickFunction.SetTickFunctionEnable(true);
}
void UGAAbilityBase::OnRep_InitAbility()
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
			ActorCue = world->SpawnActor<AGACueActor>(ActorCueClass, POwner->GetActorLocation(), FRotator(0, 0, 0), SpawnParams);
			if (ActorCue)
			{
				ActorCue->OwningAbility = this;
			}
		}
	}
}
void UGAAbilityBase::OnNativeInputPressed(FGameplayTag ActionName)
{
	//Check for cooldown and don't all inputs if on cooldown. Because.
	//if (CanUseAbility())
	{
		UE_LOG(AbilityFramework, Log, TEXT("OnNativeInputPressed in ability %s"), *GetName());
		OnInputPressed(ActionName);
	}
	//else
	//{
	//	UE_LOG(AbilityFramework, Log, TEXT("OnNativeInputPressed in ability %s is on Cooldown."), *GetName());
	//}
}

void UGAAbilityBase::OnNativeInputReleased(FGameplayTag ActionName)
{
	//if (CanReleaseAbility())
	{
		UE_LOG(AbilityFramework, Log, TEXT("OnNativeInputReleased in ability %s"), *GetName());
		OnInputReleased(ActionName);
	}
}

void UGAAbilityBase::StartActivation()
{
	if (!CanUseAbility())
	{
		return;
	}
	//AbilityComponent->ExecutingAbility = this;
	NativeOnBeginAbilityActivation();
}

void UGAAbilityBase::NativeOnBeginAbilityActivation()
{
	UE_LOG(AbilityFramework, Log, TEXT("Begin Executing Ability: %s"), *GetName());

	if (OnConfirmCastingEndedDelegate.IsBound())
	{
		OnConfirmCastingEndedDelegate.Broadcast();
	}
	//ActivationInfo.SetActivationInfo();
	ApplyActivationEffect();
	OnActivate();
	AbilityComponent->AppliedTags.AddTagContainer(ActivationAddedTags);
	//OnAbilityExecuted();
}

void UGAAbilityBase::OnCooldownEffectExpired()
{
	UE_LOG(AbilityFramework, Log, TEXT("Cooldown expired In Ability: %s"), *GetName());
	CooldownEffectExpiredCounter++;
	if (CooldownEffectHandle.IsValid())
	{
		CooldownEffectHandle.GetContextRef().InstigatorComp->RemoveEffect(CooldownEffect);
	}
}
/* Functions for activation effect delegates */
void UGAAbilityBase::NativeOnAbilityActivationFinish(const FGAEffectHandle& InHandle)
{
	//OnAbilityExecutedNative();
	UE_LOG(AbilityFramework, Log, TEXT("Ability Activation Effect Expired In Ability: %s"), *GetName());
	//AbilityComponent->ExecutingAbility = nullptr;
	//do not do it automatically.
	/*if (ActivationEffectHandle.IsValid())
	{
		ActivationEffectHandle.GetContextRef().InstigatorComp->RemoveEffect(ActivationEffectHandle);
	}*/
	OnActivationFinished();
	if (Cue)
	{
		Cue->OnAbilityActivated();
	}
}
void UGAAbilityBase::NativeOnAbilityActivationCancel()
{
	//OnAbilityExecutedNative();
	//this works under assumption that current state == activation state.
	UE_LOG(AbilityFramework, Log, TEXT("Ability Activation Effect Removed In Ability: %s"), *GetName());
	//AbilityComponent->ExecutingAbility = nullptr;
	OnConfirmDelegate.Clear();
	OnConfirmDelegate.RemoveAll(this);

	OnActivationCancel();
	//AbilityActivatedCounter++;
}
void UGAAbilityBase::OnActivationEffectPeriod(const FGAEffectHandle& InHandle)
{
	UE_LOG(AbilityFramework, Log, TEXT("Ability Activation Effect Period In Ability: %s"), *GetName());
	AbilityPeriodCounter++;
	OnAbilityPeriod();
}
void UGAAbilityBase::FinishAbility()
{
	UE_LOG(AbilityFramework, Log, TEXT("FinishExecution in ability %s"), *GetName());
	OnAbilityFinished();
	NativeFinishAbility();
	AbilityComponent->AppliedTags.RemoveTagContainer(ActivationAddedTags);
}
void UGAAbilityBase::NativeFinishAbility()
{
	UE_LOG(AbilityFramework, Log, TEXT("NativeFinishExecution in ability %s"), *GetName());
	AbilityComponent->ExecutingAbility = nullptr;
	OnConfirmDelegate.Clear();
	OnConfirmDelegate.RemoveAll(this);
	//if (ActivationEffect.Handle.IsValid())
	{
		AbilityComponent->RemoveEffect(ActivationEffect);
	}
	//remove effect.
}
/* Functions for activation effect delegates */
void UGAAbilityBase::CancelActivation()
{
	NativeCancelActivation();
}
void UGAAbilityBase::NativeCancelActivation()
{
	UGAAbilitiesComponent* AttrComp = ActivationEffect.Handle.GetContext().InstigatorComp.Get();
	if (AbilityComponent)
	{
		AbilityComponent->RemoveEffect(ActivationEffect);
	}
}

bool UGAAbilityBase::IsWaitingForConfirm()
{
	if (OnConfirmDelegate.IsBound())
		return true;
	else
		return false;
}
void UGAAbilityBase::ConfirmAbility()
{
	if (OnConfirmDelegate.IsBound())
		OnConfirmDelegate.Broadcast();
	OnConfirmDelegate.Clear();
	OnConfirmDelegate.RemoveAll(this);
}

bool UGAAbilityBase::ApplyCooldownEffect()
{
	if (!CooldownEffect.GetSpec())
	{
		return false;
	}

	return false;
}
bool UGAAbilityBase::ApplyActivationEffect()
{
	if (!ActivationEffect.GetSpec())
		return false;
	FHitResult Hit(ForceInit);
	
	UGAGameEffectSpec* Spec = ActivationEffect.GetClass().GetDefaultObject();
	FGAEffectContext ctx = UGABlueprintLibrary::MakeContext(this, POwner, this, Hit);
	float DurationCheck = Spec->Duration.GetFloatValue(ctx);
	float PeriodCheck = Spec->Period.GetFloatValue(ctx);
	if (DurationCheck > 0 || PeriodCheck > 0)
	{
		FHitResult HitIn;
		if (ActivationEffectHandle.IsValid())
			ActivationEffectHandle.Reset();

		FAFFunctionModifier Modifier;
		ActivationEffectHandle = UGABlueprintLibrary::ApplyGameEffectToObject(ActivationEffect,
			this, POwner, this, Modifier);
		
		//if(!ActivationEffectHandle.GetEffectRef().OnEffectExpired.)
			ActivationEffectHandle.GetEffectRef().OnEffectExpired.AddUObject(this, &UGAAbilityBase::NativeOnAbilityActivationFinish);

		
		if (PeriodCheck > 0)
		{
			//if (!ActivationEffectHandle.GetEffectRef().OnEffectPeriod.IsBound())
			ActivationEffectHandle.GetEffectRef().OnEffectPeriod.AddUObject(this, &UGAAbilityBase::OnActivationEffectPeriod);
		}
	}
	else
	{
		NativeOnAbilityActivationFinish(FGAEffectHandle());
	}
	return false;
}

bool UGAAbilityBase::CanUseAbility()
{
	bool CanUse = true;
	bool bIsOnCooldown = CheckCooldown();
	bool bIsActivating = CheckExecuting();
	//if (!AbilityComponent->ExecutingAbility)
	//	UE_LOG(AbilityFramework, Log, TEXT("CanUseAbility AbilityComponent->ExecutingAbility is true"));

	CanUse = !bIsOnCooldown && !bIsActivating;
	return CanUse;
}

bool UGAAbilityBase::CanReleaseAbility()
{
	bool bCanUse = true;
	/*if (AbilityComponent->ExecutingAbility == this)
	{
		bCanUse = true;
	}*/
	if (CheckCooldown())
	{
		bCanUse = false;
		UE_LOG(AbilityFramework, Log, TEXT("CanReleaseAbility can't release ability is on cooldown"));
	}
	return bCanUse;
}
float UGAAbilityBase::GetCurrentActivationTime() const
{
	if (ActivationEffect.Handle.IsValid())
	{
		return ActivationEffect.Handle.GetEffectPtr()->GetCurrentActivationTime();
	}
	return 0;
}
float UGAAbilityBase::BP_GetCurrentActivationTime() const
{
	return GetCurrentActivationTime();
}
float UGAAbilityBase::GetCurrentCooldownTime() const
{
	return 0;
}

float UGAAbilityBase::GetPeriodTime() const
{
	//if(Activation)
	return 0;
}
float UGAAbilityBase::BP_GetPeriodTime() const
{
	return GetPeriodTime();
}

float UGAAbilityBase::GetCooldownTime() const
{
	return GetWorld()->GetTimeSeconds() - LastCooldownTime;
}
float UGAAbilityBase::BP_GetCooldownTime() const
{
	return GetCooldownTime();
}
float UGAAbilityBase::GetActivationTime() const
{
	return GetWorld()->GetTimeSeconds() - LastActivationTime;
}
float UGAAbilityBase::BP_GetActivationTime() const
{
	return GetActivationTime();
}

void UGAAbilityBase::OnGameplayTaskInitialized(UGameplayTask& Task)
{
	if (UGAAbilityTask* task = Cast<UGAAbilityTask>(&Task))
	{
		task->Ability = this;
	}
}
UGameplayTasksComponent* UGAAbilityBase::GetGameplayTasksComponent(const UGameplayTask& Task) const
{
	return AbilityComponent;
}
/** this gets called both when task starts and when task gets resumed. Check Task.GetStatus() if you want to differenciate */
void UGAAbilityBase::OnGameplayTaskActivated(UGameplayTask& Task)
{
	UE_LOG(AbilityFramework, Log, TEXT("Task Started; %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Add(&Task);
	//AbilityComponent->OnGameplayTaskActivated(Task);
}
/** this gets called both when task finished and when task gets paused. Check Task.GetStatus() if you want to differenciate */
void UGAAbilityBase::OnGameplayTaskDeactivated(UGameplayTask& Task)
{
	UE_LOG(AbilityFramework, Log, TEXT("Task Removed: %s in ability: %s"), *Task.GetName(), *GetName());
	ActiveTasks.Remove(&Task);
	//AbilityComponent->OnGameplayTaskDeactivated(Task);
}
AActor* UGAAbilityBase::GetGameplayTaskOwner(const UGameplayTask* Task) const
{
	return POwner;
}
AActor* UGAAbilityBase::GetGameplayTaskAvatar(const UGameplayTask* Task) const
{
	return AvatarActor;
}

class UGAAttributesBase* UGAAbilityBase::GetAttributes()
{
	return Attributes;
}
UGAAbilitiesComponent* UGAAbilityBase::GetAbilityComp()
{
	IIGAAbilities* OwnerAttributes = Cast<IIGAAbilities>(POwner);
	if (OwnerAttributes)
	{
		return OwnerAttributes->GetAbilityComp();
	}
	return nullptr;
}
float UGAAbilityBase::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return NativeGetAttributeValue(AttributeIn);
}
float UGAAbilityBase::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return Attributes->GetCurrentAttributeValue(AttributeIn);
}
float UGAAbilityBase::GetAttributeVal(FGAAttribute AttributeIn) const
{
	return Attributes->GetCurrentAttributeValue(AttributeIn);
}
FGAEffectHandle UGAAbilityBase::ApplyEffectToTarget(FGAEffect* EffectIn,
	FGAEffectProperty& InProperty, FGAEffectContext& InContext)
{ 
	return AbilityComponent->ApplyEffectToTarget(EffectIn, InProperty, InContext);
};
void UGAAbilityBase::RemoveTagContainer(const FGameplayTagContainer& TagsIn) 
{
	AbilityComponent->RemoveTagContainer(TagsIn);
};

bool UGAAbilityBase::ApplyAttributeCost()
{
	return false;
}
bool UGAAbilityBase::ApplyAbilityAttributeCost()
{
	return true;
}
bool UGAAbilityBase::BP_ApplyAttributeCost()
{
	return ApplyAttributeCost();
}
bool UGAAbilityBase::BP_ApplyAbilityAttributeCost()
{
	return ApplyAbilityAttributeCost();
}
bool UGAAbilityBase::CheckCooldown()
{
	bool bOnCooldown = false;
	bOnCooldown = AbilityComponent->IsEffectActive(CooldownEffectHandle);
	return bOnCooldown; //temp
}
bool UGAAbilityBase::CheckExecuting()
{
	bool bAbilityActivating = false;
	bAbilityActivating = AbilityComponent->IsEffectActive(ActivationEffect.Handle);
	return bAbilityActivating; //temp
}
void UGAAbilityBase::BP_ApplyCooldown()
{
	ApplyCooldownEffect();
}

float UGAAbilityBase::GetCurrentActivationTime()
{
	if (ActivationEffectHandle.IsValid())
	{
		return ActivationEffectHandle.GetEffectPtr()->GetCurrentActivationTime();
	}
	return 0;
}

float UGAAbilityBase::CalculateAnimationSpeed(UAnimMontage* MontageIn)
{
	float ActivationTime = MontageIn->GetPlayLength();
	if (ActivationEffect.Handle.IsValid())
	{
		//ActivationTime = Attributes->GetFinalAttributeValue(ActivationTimeAttribute);// ActivationEffectHandle.GetEffectPtr()->GetActivationTime();
	}
	float Duration = MontageIn->GetPlayLength();
	
	float PlaySpeed = Duration / ActivationTime;
	return PlaySpeed;
}


bool UGAAbilityBase::IsNameStableForNetworking() const
{
	return bIsNameStable;
}

void UGAAbilityBase::SetNetAddressable()
{
	bIsNameStable = true;
}

class UWorld* UGAAbilityBase::GetWorld() const
{
	return World;
}
void UGAAbilityBase::PlayMontage(UAnimMontage* MontageIn, FName SectionName, float Speed)
{
	AbilityComponent->PlayMontage(MontageIn, SectionName, Speed);
}

void UGAAbilityBase::ActivateActorCue(FVector Location)
{
	if (ActorCue)
	{

		ActorCue->SetActorLocation(Location);
		ActorCue->SetActorHiddenInGame(false);
		ActorCue->OnActivated();
	}
}

void UGAAbilityBase::MulticastActivateActorCue_Implementation(FVector Location)
{

}

//replication
void UGAAbilityBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGAAbilityBase, POwner);
	DOREPLIFETIME(UGAAbilityBase, Character);
	DOREPLIFETIME(UGAAbilityBase, PCOwner);
	DOREPLIFETIME(UGAAbilityBase, AICOwner);
	//probabaly should be SKIP_Owner, and I'm not really sure if Multicast wouldn't be better.
	DOREPLIFETIME(UGAAbilityBase, CooldownStartedCounter);
	DOREPLIFETIME(UGAAbilityBase, CooldownEffectExpiredCounter);
	DOREPLIFETIME_CONDITION(UGAAbilityBase, ActivationInfo, COND_SkipOwner);
	//DOREPLIFETIME(UGAAbilityBase, ActivationInfo);
	DOREPLIFETIME(UGAAbilityBase, AbilityActivationStartedCounter);
	DOREPLIFETIME(UGAAbilityBase, AbilityPeriodCounter);
	DOREPLIFETIME(UGAAbilityBase, InitAbilityCounter);
	DOREPLIFETIME(UGAAbilityBase, AbilityHits);
	//DOREPLIFETIME(UGAAbilitiesComponent, RepMontage);
}
/*
	Do some yet undertemined client stuff.
	Call events ?
*/
void UGAAbilityBase::OnRep_CooldownStarted()
{

}
void UGAAbilityBase::OnRep_CooldownExpired()
{

}
void UGAAbilityBase::OnRep_AbilityActivationStarted()
{

}
void UGAAbilityBase::OnRep_AbilityActivated()
{
	ApplyActivationEffect();
}
void UGAAbilityBase::OnRep_AbilityPeriod()
{

}
void UGAAbilityBase::OnRep_AbilityHits()
{

}

void UGAAbilityBase::ExecuteAbilityInputPressedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName)
{
	AbilityComponent->NativeInputPressed(AbilityTag, ActionName);
}
void UGAAbilityBase::ExecuteAbilityInputReleasedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName)
{
	AbilityComponent->NativeInputReleased(AbilityTag, ActionName);
}

/* Tracing Helpers Start */
bool UGAAbilityBase::LineTraceSingleByChannel(const FVector Start, const FVector End, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit)
{
	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
	static const FName LineTraceSingleName(TEXT("AbilityLineTraceSingle"));
	FCollisionQueryParams Params(LineTraceSingleName, bTraceComplex);

	bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, CollisionChannel, Params);
	return bHit;
}
bool UGAAbilityBase::LineTraceSingleByChannelFromCamera(float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
	EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	FVector Start = FVector::ZeroVector;
	if (OwnerCamera)
	{
		Start = OwnerCamera->GetComponentLocation();
	}
	else
	{
		FRotator UnusedRot;
		POwner->GetActorEyesViewPoint(Start, UnusedRot);
	}
	FVector End = (POwner->GetBaseAimRotation().Vector() * Range) + Start;
	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);
	static const FName LineTraceSingleName(TEXT("AbilityLineTraceSingle"));
	FCollisionQueryParams Params(LineTraceSingleName, bTraceComplex);
	bool bHit = World->LineTraceSingleByChannel(OutHit, Start, End, CollisionChannel, Params);
#if ENABLE_DRAW_DEBUG
	if (DrawDebugType != EDrawDebugTrace::None)
	{
		bool bPersistent = DrawDebugType == EDrawDebugTrace::Persistent;
		float LifeTime = (DrawDebugType == EDrawDebugTrace::ForDuration) ? DrawTime : 0.f;

		// @fixme, draw line with thickness = 2.f?
		if (bHit && OutHit.bBlockingHit)
		{
			// Red up to the blocking hit, green thereafter
			::DrawDebugLine(World, Start, OutHit.ImpactPoint, TraceColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugLine(World, OutHit.ImpactPoint, End, TraceHitColor.ToFColor(true), bPersistent, LifeTime);
			::DrawDebugPoint(World, OutHit.ImpactPoint, 16, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
		else
		{
			// no hit means all red
			::DrawDebugLine(World, Start, End, TraceColor.ToFColor(true), bPersistent, LifeTime);
		}
	}
#endif
	return bHit;
}
bool UGAAbilityBase::LineTraceSingleByChannelFromSocket(FName SocketName, float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
	EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	return false;
}
bool UGAAbilityBase::LineTraceSingleByChannelCorrected(FName SocketName, float Range, ETraceTypeQuery TraceChannel, bool bTraceComplex, FHitResult& OutHit,
	EDrawDebugTrace::Type DrawDebugType, bool bIgnoreSelf, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	return false;
}
/* Tracing Helpers End */