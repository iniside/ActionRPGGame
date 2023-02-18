// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.


#include "AbilityFramework.h"
#include "Tasks/GAAbilityTask.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "Effects/GAEffectGlobalTypes.h"
#include "AFAbilityComponent.h"
#include "AFEffectsComponent.h"

#include "GameplayTagContainer.h"
#include "Net/UnrealNetwork.h"
#include "Animation/AnimMontage.h"
#include "Effects/GABlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "GAAbilityBase.h"
FAFPredictionHandle UGAAbilityBase::GetPredictionHandle() { return PredictionHandle; }

UGAAbilityBase::UGAAbilityBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicate = true;
	bIsNameStable = false;
}

void UGAAbilityBase::PostInitProperties()
{
	UpdateAssetRegistryInfo();
	Super::PostInitProperties();
}

void UGAAbilityBase::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		UpdateAssetRegistryInfo();
	}

	Super::Serialize(Ar);

	if (Ar.IsLoading())
	{
		UpdateAssetRegistryInfo();
	}
}
#if WITH_EDITORONLY_DATA
void UGAAbilityBase::UpdateAssetBundleData()
{
	AssetBundleData.Reset();

	// By default parse the metadata
	if (UAssetManager::IsValid())
	{
		UAssetManager::Get().InitializeAssetBundlesFromMetadata(this, AssetBundleData);
	}
}

void UGAAbilityBase::PreSave(const class ITargetPlatform* TargetPlatform)
{
	Super::PreSave(TargetPlatform);

	UpdateAssetBundleData();

	if (UAssetManager::IsValid())
	{
		// Bundles may have changed, refresh
		UAssetManager::Get().RefreshAssetData(this);
	}
}
#endif

FPrimaryAssetId UGAAbilityBase::GetPrimaryAssetId() const
{
	//FName Name = GetFName();
	//FName clsNam = GetClass()->GetFName();
	FName dupa1 = FPackageName::GetShortFName(GetOutermost()->GetFName());
	//FName dupa2 = FPackageName::GetShortFName(GetFName());
	const UGAAbilityBase* A = this;
	return FPrimaryAssetId(FPrimaryAssetType("Ability"), dupa1);
	//if (HasAnyFlags(RF_ClassDefaultObject))
	{
		UClass* SearchNativeClass = GetClass();

		while (SearchNativeClass && !SearchNativeClass->HasAnyClassFlags(CLASS_Native | CLASS_Intrinsic))
		{
			SearchNativeClass = SearchNativeClass->GetSuperClass();
		}

		if (SearchNativeClass && SearchNativeClass != GetClass())
		{
			// If blueprint, return native class and asset name
			
		}

		// Native CDO, return nothing
		return FPrimaryAssetId();
	}

	// Data assets use Class and ShortName by default, there's no inheritance so class works fine
	//return FPrimaryAssetId(GetClass()->GetFName(), GetFName());
}

void UGAAbilityBase::PostLoad()
{
	Super::PostLoad();

#if WITH_EDITORONLY_DATA
	FAssetBundleData OldData = AssetBundleData;

	UpdateAssetBundleData();

	if (UAssetManager::IsValid() && OldData != AssetBundleData)
	{
		// Bundles changed, refresh
		UAssetManager::Get().RefreshAssetData(this);
	}
#endif
}
#if WITH_EDITOR
void UGAAbilityBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

void UGAAbilityBase::UpdateAssetRegistryInfo()
{
	AbilityTagSearch = AbilityTag.GetTagName();
}

void UGAAbilityBase::InitAbility()
{
	//still want to initialize, as Spec is used in multiple places.
	DefaultContext = UGABlueprintLibrary::MakeContext(this, POwner, AvatarActor, this, FHitResult(ForceInit)).GetRef();
	ActivationEffect.InitializeIfNotInitialized(POwner, this);
	CooldownEffect.InitializeIfNotInitialized(POwner, this);
	for (int32 Idx = 0; Idx < AttributeCost.Num(); Idx++)
	{
		AttributeCost[Idx].InitializeIfNotInitialized(POwner, this);
	}
	AttributeCostHandle.AddZeroed(AttributeCost.Num());

	for (int32 Idx = 0; Idx < AbilityAttributeCost.Num(); Idx++)
	{
		AbilityAttributeCost[Idx].InitializeIfNotInitialized(POwner, this);
	}
	AbilityAttributeCostHandle.AddZeroed(AbilityAttributeCost.Num());
	if (AbilityComponent)
	{
		World = AbilityComponent->GetWorld();
	}

	if (!AbilityComponent)
	{
		AbilityComponent = GetAbilityComp();
	}
	
	if (GetAttributes())
	{
		GetAttributes()->InitializeAttributes(GetAbilityComp());
		GetAttributes()->InitializeAttributesFromTable();
	}
	ENetRole role = AbilityComponent->GetOwnerRole();
	ENetMode mode = AbilityComponent->GetOwner()->GetNetMode();
	
	
	{ 
		FAFOnAttributeReady Delegate = FAFOnAttributeReady::CreateUObject(this, &UGAAbilityBase::OnAttributeSetReplicated);
		AbilityComponent->RepAttributes.RegisterAttributeRepEvent(AbilityTag, Delegate);
	}

	//AbilityComponent->RepAttributes.AttributeMap.Add(AbilityTag, Attributes);
	if (role == ENetRole::ROLE_Authority ||
		mode == ENetMode::NM_Standalone)
	{
		if (AbilityComponent && GetAttributes())
		{
			UGAAttributesBase* NewAttributes = AbilityComponent->AddAddtionalAttributes(AbilityTag, GetAttributes());;
			SetAttributes(nullptr);
			SetAttributes(NewAttributes);
		}
	}
	
	if (!OwnerCamera)
	{
		OwnerCamera = POwner->FindComponentByClass<UCameraComponent>();
	}

	OnAbilityInited();
}
void UGAAbilityBase::OnAttributeSetReplicated(class UGAAttributesBase* ReplicatedAttributes)
{
	SetAttributes(ReplicatedAttributes);
	if (GetAttributes())
	{
		GetAttributes()->InitializeAttributes(GetAbilityComp());
		GetAttributes()->InitializeAttributesFromTable();
	}
}

void UGAAbilityBase::OnAbilityInited()
{

}
void UGAAbilityBase::OnNativeInputPressed(uint8 InputID, const FAFPredictionHandle& InPredictionHandle)
{
	{
		//UE_LOG(AbilityFramework, Log, TEXT("OnNativeInputPressed in ability %s"), *GetName());
		PredictionHandle = InPredictionHandle;
		OnInputPressed(InputID);
		OnInputPressedDelegate.Broadcast();
	}
}

void UGAAbilityBase::OnNativeInputReleased(uint8 InputID)
{
	{
		UE_LOG(AbilityFramework, Log, TEXT("OnNativeInputReleased in ability %s"), *GetName());
		OnInputReleased(InputID);
		OnInputReleasedDelegate.Broadcast();
	}
}

void UGAAbilityBase::StartActivation(bool bApplyActivationEffect)
{
	if (!CanUseAbility())
	{
		UE_LOG(AbilityFramework, Log, TEXT("Cannot use Ability: %s"), *GetName());
		return;
	}
	//AbilityComponent->ExecutingAbility = this;
	AbilityState = EAFAbilityState::Activating;
	NativeOnBeginAbilityActivation(bApplyActivationEffect);
}

void UGAAbilityBase::NativeOnBeginAbilityActivation(bool bApplyActivationEffect)
{
	UE_LOG(AbilityFramework, Log, TEXT("Begin Executing Ability: %s"), *GetName());

	if (OnConfirmCastingEndedDelegate.IsBound())
	{
		OnConfirmCastingEndedDelegate.Broadcast();
	}
	//ActivationInfo.SetActivationInfo();
	ApplyActivationEffect(bApplyActivationEffect);
	OnActivate();
	OnActivateBeginDelegate.Broadcast();
	AbilityComponent->AppliedTags.AddTagContainer(ActivationAddedTags);
	//OnAbilityExecuted();
}

void UGAAbilityBase::OnCooldownEffectExpired()
{
	UE_LOG(AFAbilities, Log, TEXT("Cooldown expired In Ability: %s"), *GetName());
}
/* Functions for activation effect delegates */
void UGAAbilityBase::NativeOnAbilityActivationFinish()
{
	UE_LOG(AbilityFramework, Log, TEXT("Ability Activation Effect Expired In Ability: %s"), *GetName());
	OnActivationFinished();
	OnActivationFinishedDelegate.Broadcast();
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
void UGAAbilityBase::OnActivationEffectPeriod()
{
	UE_LOG(AFAbilities, Log, TEXT("Ability Activation Effect Period In Ability: %s"), *GetName());

	OnPeriod();
}
void UGAAbilityBase::FinishAbility()
{
	UE_LOG(AFAbilities, Log, TEXT("FinishExecution in ability %s"), *GetName());
	OnAbilityFinished();
	NativeFinishAbility();
	AbilityState = EAFAbilityState::Waiting;
	GetEffectsComponent()->AppliedTags.RemoveTagContainer(ActivationAddedTags);
}
void UGAAbilityBase::NativeFinishAbility()
{
	UE_LOG(AFAbilities, Log, TEXT("NativeFinishExecution in ability %s"), *GetName());
	AbilityComponent->ExecutingAbility = nullptr;
	OnConfirmDelegate.Clear();
	OnConfirmDelegate.RemoveAll(this);
	if (ActivationEffectHandle.Num() > 0 &&  !ActivationEffectHandle[0].IsValid())
	{
		GetEffectsComponent()->RemoveEffect(ActivationEffect, DefaultContext, ActivationEffectHandle[0]);
		ActivationEffectHandle.Reset();
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
	if (ActivationEffectHandle.Num() > 0 && !ActivationEffectHandle[0].IsValid())
		return;

	UAFAbilityComponent* AttrComp = DefaultContext.InstigatorComp.Get();
	AbilityComponent->ExecutingAbility = nullptr;
	OnConfirmDelegate.Clear();
	OnConfirmDelegate.RemoveAll(this);
	if (GetEffectsComponent())
	{
		GetEffectsComponent()->RemoveEffect(ActivationEffect, DefaultContext, ActivationEffectHandle[0]);
		AbilityState = EAFAbilityState::Waiting;
		ActivationEffectHandle.Reset();
	}
	OnActivationCancel();
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
	if (!CooldownEffect.GetSpecData())
	{
		return false;
	}
	FAFFunctionModifier Modifier;

	FSimpleDelegate PeriodDel = FSimpleDelegate::CreateUObject(this, &UGAAbilityBase::NativeOnCooldownEnd);
	GetEffectsComponent()->AddEffectEvent(CooldownEffect.GetSpecData()->OnExpiredEvent, PeriodDel);

	CooldownEffectHandle = UGABlueprintLibrary::ApplyGameEffectToObject(
		CooldownEffect
		, this
		, POwner
		, this
		, Modifier);
	ENetMode nm = AbilityComponent->GetOwner()->GetNetMode();
	ENetRole role = AbilityComponent->GetOwnerRole();

	if (role >= ENetRole::ROLE_Authority)
	{
		ClientSetCooldownHandle(CooldownEffectHandle[0]);
	}
	OnCooldownStart();

	//CooldownEffectHandle.GetEffectRef().OnEffectExpired.AddUObject(this, &UGAAbilityBase::OnCooldownEnd);
	return false;
}
void UGAAbilityBase::NativeOnCooldownEnd()
{
	//AbilityComponent->RemoveEffect(CooldownEffect, DefaultContext);
	OnCooldownEnd();
	CooldownEffectHandle.Reset();
}
void UGAAbilityBase::ClientSetCooldownHandle_Implementation(FGAEffectHandle InCooldownHandle)
{
	//CooldownEffectHandle = InCooldownHandle;
}
bool UGAAbilityBase::ApplyActivationEffect(bool bApplyActivationEffect)
{
	if (!ActivationEffect.GetSpecData())
		return false;
	FHitResult Hit(ForceInit);
	
	UGAGameEffectSpec* Spec = ActivationEffect.GetClass().GetDefaultObject();
	float DurationCheck = Spec->Duration.GetFloatValue(DefaultContext);
	float PeriodCheck = Spec->Period.GetFloatValue(DefaultContext);
	if (DurationCheck > 0 || PeriodCheck > 0)
	{
		bApplyActivationEffect = true;
	}
	
	if (bApplyActivationEffect)
	{
		FHitResult HitIn;

		FSimpleDelegate AppliedDel = FSimpleDelegate::CreateUObject(this, &UGAAbilityBase::NativeOnAbilityActivationFinish);
		GetEffectsComponent()->AddEffectEvent(ActivationEffect.GetSpecData()->OnExpiredEvent, AppliedDel);

		FAFFunctionModifier Modifier;
		ActivationEffectHandle = UGABlueprintLibrary::ApplyGameEffectToObject(
			ActivationEffect
			, this
			, POwner
			, this
			, Modifier);
		
		//if(!ActivationEffectHandle.GetEffectRef().OnEffectExpired.)
		//	ActivationEffectHandle.GetEffectRef().OnEffectExpired.AddUObject(this, &UGAAbilityBase::NativeOnAbilityActivationFinish);

		
		if (PeriodCheck > 0)
		{
			FSimpleDelegate PeriodDel = FSimpleDelegate::CreateUObject(this, &UGAAbilityBase::OnActivationEffectPeriod);
			GetEffectsComponent()->AddEffectEvent(ActivationEffect.GetSpecData()->OnPeriodEvent, PeriodDel);
			//if (!ActivationEffectHandle.GetEffectRef().OnEffectPeriod.IsBound())
		//	ActivationEffectHandle.GetEffectRef().OnEffectPeriod.AddUObject(this, &UGAAbilityBase::OnActivationEffectPeriod);
		}
	}
	else
	{
		NativeOnAbilityActivationFinish();
	}
	return false;
}

bool UGAAbilityBase::CanUseAbility()
{
	bool CanUse = true;
	bool bIsOnCooldown = IsOnCooldown();
	bool bIsActivating = IsActivating();
	//if (!AbilityComponent->ExecutingAbility)
	//	UE_LOG(AbilityFramework, Log, TEXT("CanUseAbility AbilityComponent->ExecutingAbility is true"));

	CanUse = !bIsOnCooldown && !bIsActivating;
	UE_LOG(AbilityFramework, Log, TEXT("CanUseAbility Ability, Cooldown: %s, Activating: %s \n"), bIsOnCooldown ? TEXT("true") : TEXT("false"), bIsActivating ? TEXT("true") : TEXT("false") );
	//now Lets check tags
	if (CanUse)
	{
		if (GetEffectsComponent()->HasAll(ActivationRequiredTags))
		{
			CanUse = true;
		}
		//blocking takes precedence.
		if (GetEffectsComponent()->HasAny(ActivationBlockedTags))
		{
			CanUse = false;
		}
	}

	return CanUse;
}
bool UGAAbilityBase::BP_CanUseAbility()
{
	return CanUseAbility();
}
bool UGAAbilityBase::CanReleaseAbility()
{
	bool bCanUse = true;
	/*if (AbilityComponent->ExecutingAbility == this)
	{
		bCanUse = true;
	}*/
	if (IsOnCooldown())
	{
		bCanUse = false;
		UE_LOG(AbilityFramework, Log, TEXT("CanReleaseAbility can't release ability is on cooldown"));
	}
	return bCanUse;
}

class UGAAttributesBase* UGAAbilityBase::GetAttributes()
{
	return Attributes;
}
UAFAbilityComponent* UGAAbilityBase::GetAbilityComp()
{
	IAFAbilityInterface* OwnerAttributes = Cast<IAFAbilityInterface>(POwner);
	if (OwnerAttributes)
	{
		return OwnerAttributes->GetAbilityComp();
	}
	return nullptr;
}

UAFEffectsComponent* UGAAbilityBase::GetEffectsComponent()
{
	IAFAbilityInterface* OwnerAttributes = Cast<IAFAbilityInterface>(POwner);
	if (OwnerAttributes)
	{
		return OwnerAttributes->GetEffectsComponent();
	}
	return nullptr;
}
UAFEffectsComponent* UGAAbilityBase::NativeGetEffectsComponent() const
{
	IAFAbilityInterface* OwnerAttributes = Cast<IAFAbilityInterface>(POwner);
	if (OwnerAttributes)
	{
		return OwnerAttributes->NativeGetEffectsComponent();
	}
	return nullptr;
}
float UGAAbilityBase::GetAttributeValue(FGAAttribute AttributeIn) const
{
	return NativeGetAttributeValue(AttributeIn);
}
float UGAAbilityBase::NativeGetAttributeValue(const FGAAttribute AttributeIn) const
{
	return GetAttributes()->GetCurrentAttributeValue(AttributeIn);
}
float UGAAbilityBase::GetAttributeVal(FGAAttribute AttributeIn) const
{
	return GetAttributes()->GetCurrentAttributeValue(AttributeIn);
}

bool UGAAbilityBase::ApplyAttributeCost()
{
	FAFFunctionModifier Modifier;
	if (CheckAttributeCost())
	{
		for (int32 Idx = 0; Idx < AttributeCost.Num(); Idx++)
		{
			AttributeCostHandle[Idx] = UGABlueprintLibrary::ApplyGameEffectToObject(
				AttributeCost[Idx]
				, POwner
				, POwner
				, this
				, Modifier);
		}
		return true;
	}
	return false;
}
bool UGAAbilityBase::ApplyAbilityAttributeCost()
{
	//add checking if attribute goes below zero
	//maybe let game specific code handle it..
	FAFFunctionModifier Modifier;
	
	if (CheckAbilityAttributeCost())
	{
		for (int32 Idx = 0; Idx < AbilityAttributeCost.Num(); Idx++)
		{
			AbilityAttributeCostHandle[Idx] = UGABlueprintLibrary::ApplyGameEffectToObject(
				AbilityAttributeCost[Idx]
				, this
				, POwner
				, this
				, Modifier);
		}
		return true;
	}
	return false;
}
bool UGAAbilityBase::BP_ApplyAttributeCost()
{
	return ApplyAttributeCost();
}
bool UGAAbilityBase::BP_CheckAttributeCost()
{
	return CheckAttributeCost();
}

bool UGAAbilityBase::BP_ApplyAbilityAttributeCost()
{
	return ApplyAbilityAttributeCost();
}
bool UGAAbilityBase::BP_CheckAbilityAttributeCost()
{
	return CheckAbilityAttributeCost();
}
bool UGAAbilityBase::CheckAbilityAttributeCost()
{
	for (int32 Idx = 0; Idx < AbilityAttributeCost.Num(); Idx++)
	{
		float ModValue = AbilityAttributeCost[Idx].GetSpecData()->AtributeModifier.Magnitude.GetFloatValue(DefaultContext);
		FGAAttribute Attribute = AbilityAttributeCost[Idx].GetSpecData()->AtributeModifier.Attribute;
		float AttributeVal = GetAttributes()->GetFloatValue(Attribute);
		if (ModValue > AttributeVal)
			return false;
	}
	return true;
}
bool UGAAbilityBase::CheckAttributeCost()
{
	IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(POwner);

	for (int32 Idx = 0; Idx < AttributeCost.Num(); Idx++)
	{
		float ModValue = AttributeCost[Idx].GetSpecData()->AtributeModifier.Magnitude.GetFloatValue(DefaultContext);
		FGAAttribute Attribute = AttributeCost[Idx].GetSpecData()->AtributeModifier.Attribute;
		float AttributeVal = Interface->GetAttributes()->GetFloatValue(Attribute);
		if (ModValue > AttributeVal)
			return false;
	}
	return true;
}
bool UGAAbilityBase::IsOnCooldown()
{
	if (CooldownEffectHandle.Num() <= 0)
		return false;

	bool bOnCooldown = false;
	bOnCooldown = GetEffectsComponent()->IsEffectActive(CooldownEffectHandle[0]);
	if (bOnCooldown)
	{
		OnNotifyOnCooldown.Broadcast();
	}
	return bOnCooldown; //temp
}
bool UGAAbilityBase::IsActivating()
{
	if (ActivationEffectHandle.Num() <= 0)
		return false;

	bool bAbilityActivating = false;
	bool bHaveEffect = GetEffectsComponent()->IsEffectActive(ActivationEffectHandle[0]);
	bool bInActivatingState = AbilityState == EAFAbilityState::Activating;
	UE_LOG(AbilityFramework, Log, TEXT("IsActivating Ability, Effect: %s, State: %s \n"), bHaveEffect ? TEXT("true") : TEXT("false"), bInActivatingState ? TEXT("true") : TEXT("false"));
	bAbilityActivating = bHaveEffect || bInActivatingState;
	
	return bAbilityActivating; //temp
}
bool UGAAbilityBase::BP_IsOnCooldown()
{
	return IsOnCooldown();
}
void UGAAbilityBase::BP_ApplyCooldown()
{
	ApplyCooldownEffect();
}

float UGAAbilityBase::GetCurrentActivationTime()
{
	if (ActivationEffectHandle.Num() <= 0)
	{
		return 0;
	}

	if (ActivationEffectHandle[0].IsValid())
	{
		if (IAFAbilityInterface* Interface = DefaultContext.TargetInterface)
		{
			FGAEffect* Effect = Interface->GetEffectsComponent()->GetEffect(ActivationEffectHandle[0]);
			if (Effect)
			{
				return Effect->GetCurrentDuration();
			}
		}
		//return ActivationEffectHandle.GetEffectPtr()->GetCurrentDuration();
	}
	return 0;
}

float UGAAbilityBase::CalculateAnimationSpeed(UAnimMontage* MontageIn)
{
	float ActivationTime = MontageIn->GetPlayLength();
	UGAGameEffectSpec* Spec = ActivationEffect.GetClass().GetDefaultObject();
	float DurationCheck = Spec->Duration.GetFloatValue(DefaultContext);
	if (DurationCheck > 0)
	{
		ActivationTime = DurationCheck;
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

//replication
void UGAAbilityBase::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//possibly can be infered upon replication.
	//does other players need info about this ?
	DOREPLIFETIME(UGAAbilityBase, POwner);
	DOREPLIFETIME(UGAAbilityBase, PCOwner);
	DOREPLIFETIME(UGAAbilityBase, AICOwner);
	DOREPLIFETIME(UGAAbilityBase, AvatarActor);
	//probabaly should be SKIP_Owner, and I'm not really sure if Multicast wouldn't be better.

}
int32 UGAAbilityBase::GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack)
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return FunctionCallspace::Local;
	}
	check(POwner != NULL);
	return POwner->GetFunctionCallspace(Function, Parameters, Stack);
}
bool UGAAbilityBase::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	check(!HasAnyFlags(RF_ClassDefaultObject));
	check(POwner != NULL);

	
	UNetDriver* NetDriver = POwner->GetNetDriver();
	if (NetDriver)
	{
		NetDriver->ProcessRemoteFunction(POwner, Function, Parameters, OutParms, Stack, this);
		return true;
	}

	return false;
}

void UGAAbilityBase::ExecuteAbilityInputPressedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName)
{
	//AbilityComponent->NativeInputPressed(ActionName);
}
void UGAAbilityBase::ExecuteAbilityInputReleasedFromTag(FGameplayTag AbilityTagIn, FGameplayTag ActionName)
{
	//AbilityComponent->NativeInputReleased(ActionName);
}

bool UGAAbilityBase::HaveGameplayTag(AActor* Target, const FGameplayTag& Tag)
{
	bool bHaveTag = false;
	if (IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(Target))
	{
		if (UAFEffectsComponent* Comp = GetEffectsComponent())
		{
			if (Comp->HasTag(Tag))
			{
				bHaveTag = true;
			}
		}
	}
	return bHaveTag;
}
bool UGAAbilityBase::HaveAnyGameplayTag(AActor* Target, const FGameplayTagContainer& Tag)
{
	bool bHaveTag = false;
	if (IAFAbilityInterface* Interface = Cast<IAFAbilityInterface>(Target))
	{
		if (UAFEffectsComponent* Comp = GetEffectsComponent())
		{
			if (Comp->HasAny(Tag))
			{
				bHaveTag = true;
			}
		}
	}
	return bHaveTag;
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

//Helpers
float UGAAbilityBase::GetActivationRemainingTime() const
{
	if (ActivationEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetRemainingTime(ActivationEffectHandle[0]);
}
float UGAAbilityBase::GetActivationRemainingTimeNormalized() const
{
	if (ActivationEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetRemainingTimeNormalized(ActivationEffectHandle[0]);
}
float UGAAbilityBase::GetActivationCurrentTime() const
{
	if (ActivationEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetCurrentTime(ActivationEffectHandle[0]);
}
float UGAAbilityBase::GetActivationCurrentTimeNormalized() const
{
	if (ActivationEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetCurrentTimeNormalized(ActivationEffectHandle[0]);
}
float UGAAbilityBase::GetActivationEndTime() const
{
	if (ActivationEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetEndTime(ActivationEffectHandle[0]);
}
float UGAAbilityBase::BP_GetActivationRemainingTime()
{
	return GetActivationRemainingTime();
}
float UGAAbilityBase::BP_GetActivationRemainingTimeNormalized()
{
	return GetActivationRemainingTimeNormalized();
}
float UGAAbilityBase::BP_GetActivationCurrentTime()
{
	return GetActivationCurrentTime();
}
float UGAAbilityBase::BP_GetActivationCurrentTimeNormalized()
{
	return GetActivationCurrentTimeNormalized();
}
float UGAAbilityBase::BP_GetActivationEndTime()
{
	return GetActivationEndTime();
}


float UGAAbilityBase::GetCooldownRemainingTime() const
{
	if (CooldownEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetRemainingTime(CooldownEffectHandle[0]);
}
float UGAAbilityBase::GetCooldownRemainingTimeNormalized() const
{
	if (CooldownEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetRemainingTimeNormalized(CooldownEffectHandle[0]);
}
float UGAAbilityBase::GetCooldownCurrentTime() const
{
	if (CooldownEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetCurrentTime(CooldownEffectHandle[0]);
}
float UGAAbilityBase::GetCooldownCurrentTimeNormalized() const
{
	if (CooldownEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetCurrentTimeNormalized(CooldownEffectHandle[0]);
}
float UGAAbilityBase::GetCooldownEndTime() const
{
	if (CooldownEffectHandle.Num() <= 0)
		return 0;
	return NativeGetEffectsComponent()->GameEffectContainer.GetEndTime(CooldownEffectHandle[0]);
}
float UGAAbilityBase::BP_GetCooldownRemainingTime()
{
	return GetCooldownRemainingTime();
}
float UGAAbilityBase::BP_GetCooldownRemainingTimeNormalized()
{
	return GetCooldownRemainingTimeNormalized();
}
float UGAAbilityBase::BP_GetCooldownCurrentTime()
{
	return GetCooldownCurrentTime();
}
float UGAAbilityBase::BP_GetCooldownCurrentTimeNormalized()
{
	return GetCooldownCurrentTimeNormalized();
}
float UGAAbilityBase::BP_GetCooldownEndTime()
{
	return GetCooldownEndTime();
}

AActor*  UGAAbilityBase::BP_GetAvatar()
{
	return AvatarActor;
}


void UGAAbilityBase::OnLatentTaskAdded(FName InstanceName, class UAFTaskBase* TaskIn)
{
	if (!InstanceName.IsNone())
	{
		AbilityTasks.Add(InstanceName, TaskIn);
	}
};
void UGAAbilityBase::AddReplicatedTask(class UAFTaskBase* TaskIn)
{
	AbilityComponent->ReplicatedTasks.Add(TaskIn);
}
void UGAAbilityBase::OnLatentTaskRemoved(class UAFTaskBase* TaskIn)
{
};
	 
void UGAAbilityBase::OnLatentTaskActivated(class UAFTaskBase* TaskIn)
{
};
void UGAAbilityBase::OnLatentTaskDeactivated(class UAFTaskBase* TaskIn)
{
};

class UAFTaskBase* UGAAbilityBase::GetCachedLatentAction(FName TaskName)
{
	return AbilityTasks.FindRef(TaskName);
}
class UGAAbilityTask* UGAAbilityBase::GetAbilityTask(const FName& InName)
{
	UAFTaskBase* result = AbilityTasks.FindRef(InName);
	return Cast<UGAAbilityTask>(result);
}
