#pragma once
#include "AbilityFramework.h"
#include "Effects/GAGameEffect.h"
#include "GAGlobalTypes.h"
#include "GameplayTagContainer.h"
#include "AFAbilityComponent.h"
#include "Attributes/GAAttributeBase.h"
#include "Effects/GAEffectExecution.h"
#include "AFAbilityInterface.h"
#include "Effects/GACustomCalculation.h"

void AddLogDebugInfo(FString Text, UWorld* World)
{
	bool bIsServer = GEngine->GetNetMode(World) == ENetMode::NM_DedicatedServer;
	UE_LOG(AbilityFramework, Log, TEXT("%s :: %s"), bIsServer ? TEXT("Server") : TEXT("Client"), *Text);
}

FGAEffectContext::FGAEffectContext(TWeakObjectPtr<class UGAAttributesBase> TargetAttributesIn, TWeakObjectPtr<class UGAAttributesBase> InstigatorAttributesIn,
	const FVector& TargetHitLocationIn, TWeakObjectPtr<UObject> TargetIn,
	TWeakObjectPtr<UObject> CauserIn, TWeakObjectPtr<APawn> InstigatorIn,
	TWeakObjectPtr<class UAFAbilityComponent> TargetCompIn,
	TWeakObjectPtr<class UAFAbilityComponent> InstigatorCompIn,
	TWeakObjectPtr<class AActor> InAvatar)
	: TargetHitLocation(TargetHitLocationIn), 
	TargetAttributes(TargetAttributesIn),
	InstigatorAttributes(InstigatorAttributesIn),
	Target(TargetIn),
	Causer(CauserIn),
	Instigator(InstigatorIn),
	Avatar(InAvatar),
	TargetComp(TargetCompIn),
	InstigatorComp(InstigatorCompIn)
{
	TargetInterface = Cast<IAFAbilityInterface>(TargetIn.Get());
	InstigatorInterface = Cast<IAFAbilityInterface>(Instigator.Get());
	IAFAbilityInterface* CauserInterface = Cast<IAFAbilityInterface>(Causer.Get());
}
FGAEffectHandle::FGAEffectHandle(uint32 HandleIn, FGAEffect* EffectIn)
	: Handle(HandleIn),
	EffectPtr(EffectIn)
{
}

FGAEffectHandle::~FGAEffectHandle()
{
	Reset();
}
FGAEffectContext& FGAEffectHandle::GetContextRef() { return EffectPtr->Context; }
FGAEffectContext& FGAEffectHandle::GetContextRef() const { return EffectPtr->Context; }

UGAGameEffectSpec* FGAEffectHandle::GetEffectSpec() { return EffectPtr->GameEffect; }
UGAGameEffectSpec* FGAEffectHandle::GetEffectSpec() const { return EffectPtr->GameEffect; }

uint32 FGAEffectHandle::GetHandle() const { return Handle; }

FGAEffect FGAEffectHandle::GetEffect() { return *EffectPtr.Get(); }
FGAEffect FGAEffectHandle::GetEffect() const { return *EffectPtr.Get(); }

FGAEffect& FGAEffectHandle::GetEffectRef() { return EffectPtr.ToSharedRef().Get(); }
FGAEffect& FGAEffectHandle::GetEffectRef() const { return EffectPtr.ToSharedRef().Get(); };

TSharedPtr<FGAEffect> FGAEffectHandle::GetEffectPtr() { return EffectPtr; };
TSharedPtr<FGAEffect> FGAEffectHandle::GetEffectPtr() const { return EffectPtr; };

void FGAEffectHandle::SetContext(const FGAEffectContext& ContextIn) { EffectPtr->SetContext(ContextIn); }
void FGAEffectHandle::SetContext(const FGAEffectContext& ContextIn) const { EffectPtr->SetContext(ContextIn); }

FGAEffectContext& FGAEffectHandle::GetContext() { return EffectPtr->Context; }
FGAEffectContext& FGAEffectHandle::GetContext() const { return EffectPtr->Context; }

/* Executes effect trough provided execution class. */

FGAEffectHandle FGAEffectHandle::GenerateHandle(FGAEffect* EffectIn)
{
	static uint32 Handle;
	Handle++;
	return FGAEffectHandle(Handle, EffectIn);
}
void FGAEffectHandle::AppendOwnedTags(const FGameplayTagContainer& TagsIn)
{
	GetEffectPtr()->OwnedTags.AppendTags(TagsIn);
}
void FGAEffectHandle::AppendOwnedTags(const FGameplayTagContainer& TagsIn) const
{
	GetEffectPtr()->OwnedTags.AppendTags(TagsIn);
}

bool FGAEffectHandle::HasAllTags(const FGameplayTagContainer& TagsIn) const
{
	return EffectPtr->OwnedTags.HasAll(TagsIn);
}
bool FGAEffectHandle::HasAllTagsExact(const FGameplayTagContainer& TagsIn) const
{
	return EffectPtr->OwnedTags.HasAllExact(TagsIn);
}
bool FGAEffectHandle::HasAllAttributeTags(const FGAEffectHandle& HandleIn) const
{
	return GetEffectSpec()->AttributeTags.HasAll(HandleIn.GetEffectSpec()->AttributeTags);
}
bool FGAEffectHandle::HasAllAttributeTagsExact(const FGAEffectHandle& HandleIn) const
{
	return GetEffectSpec()->AttributeTags.HasAllExact(HandleIn.GetEffectSpec()->AttributeTags);
}
FGameplayTagContainer& FGAEffectHandle::GetOwnedTags() const
{
	return EffectPtr->OwnedTags;
}
FGAEffectMod FGAEffectHandle::GetAttributeModifier() const
{
	return FGAEffectMod();// EffectPtr->GetAttributeModifier();
}

FGAAttribute FGAEffectHandle::GetAttribute() const
{
	return GetEffectSpec()->AtributeModifier.Attribute;
}
EGAAttributeMod FGAEffectHandle::GetAttributeMod() const
{
	return GetEffectSpec()->AtributeModifier.AttributeMod;
}

bool FGAEffectHandle::IsValid() const
{
	return (Handle != INDEX_NONE) && EffectPtr.IsValid();// && EffectPtr->Context.IsValid();
}
//void FGAEffectHandle::operator=(const FGAEffectHandle& Other)
//{
//	Handle = Other.Handle;
//	//EffectPtr = Other.EffectPtr;
//}
void FGAEffectHandle::Reset()
{
	Handle = INDEX_NONE;
	EffectPtr.Reset();
}

FAFPredictionHandle FAFPredictionHandle::GenerateClientHandle(UAFAbilityComponent* InComponent)
{
	if (InComponent->GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		static uint32 Counter = 1;
		Counter++;
		FAFPredictionHandle Handle;
		Handle.Handle = Counter;
		return Handle;
	}
	return FAFPredictionHandle();
}

FGAHashedGameplayTagContainer::FGAHashedGameplayTagContainer(const FGameplayTagContainer& TagsIn)
	: Tags(TagsIn)
{
	GenerateFNameKey();
}
void FGAHashedGameplayTagContainer::GenerateFNameKey()
{
	FString RetString;
	for (const FGameplayTag& tag : Tags)
	{
		RetString += TEXT(".");
		RetString += tag.ToString();
		RetString += TEXT(".");
	}
	Key = *RetString;
}

void FGAEffectContext::Reset()
{
	Target.Reset();
	Causer.Reset();
	Instigator.Reset();
	TargetComp.Reset();
	InstigatorComp.Reset();
}
class UGAAttributesBase* FGAEffectContext::GetTargetAttributes()
{ 
	if (TargetAttributes.IsValid())
		return TargetAttributes.Get();
	else
		return nullptr;
}
class UGAAttributesBase* FGAEffectContext::GetInstigatorAttributes()
{ 
	if(InstigatorComp.IsValid())
		return InstigatorComp->DefaultAttributes; 
	return nullptr;
}
class UGAAttributesBase* FGAEffectContext::GetCauserAttributes()
{
	IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Causer.Get());
	if (AttrInt)
	{
		return AttrInt->GetAttributes();
	}
	return nullptr;
}

class UGAAttributesBase* FGAEffectContext::GetTargetAttributes() const
{
	if (TargetAttributes.IsValid())
		return TargetAttributes.Get();
	else
		return nullptr;
}
class UGAAttributesBase* FGAEffectContext::GetInstigatorAttributes() const
{
	if (InstigatorComp.IsValid())
		return InstigatorComp->DefaultAttributes;
	return nullptr;
}
class UGAAttributesBase* FGAEffectContext::GetCauserAttributes() const
{
	IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Causer.Get());
	if (AttrInt)
	{
		return AttrInt->GetAttributes();
	}
	return nullptr;
}

class UAFEffectsComponent* FGAEffectContext::GetTargetEffectsComponent()
{
	IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Target.Get());
	if (AttrInt)
	{
		return AttrInt->GetEffectsComponent();
	}
	return nullptr;
}

class UAFEffectsComponent* FGAEffectContext::GetTargetEffectsComponent() const
{
	IAFAbilityInterface* AttrInt = Cast<IAFAbilityInterface>(Target.Get());
	if (AttrInt)
	{
		return AttrInt->GetEffectsComponent();
	}
	return nullptr;
}

FGAEffectContext::~FGAEffectContext()
{
	Target.Reset();
	Causer.Reset();
	Instigator.Reset();
	TargetComp.Reset();
	InstigatorComp.Reset();
}

void FGACountedTagContainer::AddTag(const FGameplayTag& TagIn)
{
	int32& count = CountedTags.FindOrAdd(TagIn);
	//if (count)
	//{
	//	*count += 1;
	//	return;
	//}
	count++;
	//CountedTags.Add(TagIn, 1);
	AllTags.AddTag(TagIn);
}
void FGACountedTagContainer::AddTagContainer(const FGameplayTagContainer& TagsIn)
{
	for (auto TagIt = TagsIn.CreateConstIterator(); TagIt; ++TagIt)
	{
		int32* count = CountedTags.Find(*TagIt);
		if (count)
		{
			*count += 1;
		}
		else
		{
			CountedTags.Add(*TagIt, 1);
			AllTags.AddTag(*TagIt);
		}
	}
}
void FGACountedTagContainer::RemoveTag(const FGameplayTag& TagIn)
{
	int32* count = CountedTags.Find(TagIn);
	if (count)
	{
		*count -= 1;
		if (*count <= 0)
		{
			CountedTags.Remove(TagIn);
			AllTags.RemoveTag(TagIn);
		}
	}
}
void FGACountedTagContainer::RemoveTagContainer(const FGameplayTagContainer& TagsIn)
{
	for (auto TagIt = TagsIn.CreateConstIterator(); TagIt; ++TagIt)
	{
		int32* count = CountedTags.Find(*TagIt);
		if (count)
		{
			*count -= 1;
		}
		if (*count <= 0)
		{
			CountedTags.Remove(*TagIt);
			AllTags.RemoveTag(*TagIt);
		}
	}
}
bool FGACountedTagContainer::HasTag(const FGameplayTag& TagIn)
{
	return AllTags.HasTag(TagIn);
}
bool FGACountedTagContainer::HasTagExact(const FGameplayTag TagIn)
{
	return AllTags.HasTagExact(TagIn);
}
bool FGACountedTagContainer::HasAny(const FGameplayTagContainer& TagsIn)
{
	return AllTags.HasAny(TagsIn);
}
bool FGACountedTagContainer::HasAnyExact(const FGameplayTagContainer& TagsIn)
{
	return AllTags.HasAnyExact(TagsIn);
}
bool FGACountedTagContainer::HasAll(const FGameplayTagContainer& TagsIn)
{
	return AllTags.HasAll(TagsIn);
}
bool FGACountedTagContainer::HasAllExact(const FGameplayTagContainer& TagsIn)
{
	return AllTags.HasAllExact(TagsIn);
}

bool FGACountedTagContainer::HasTag(const FGameplayTag& TagIn) const
{
	return AllTags.HasTag(TagIn);
}
bool FGACountedTagContainer::HasTagExact(const FGameplayTag TagIn) const
{
	return AllTags.HasTagExact(TagIn);
}
bool FGACountedTagContainer::HasAny(const FGameplayTagContainer& TagsIn) const
{
	return AllTags.HasAny(TagsIn);
}
bool FGACountedTagContainer::HasAnyExact(const FGameplayTagContainer& TagsIn) const
{
	return AllTags.HasAnyExact(TagsIn);
}
bool FGACountedTagContainer::HasAll(const FGameplayTagContainer& TagsIn) const
{
	return AllTags.HasAll(TagsIn);
}
bool FGACountedTagContainer::HasAllExact(const FGameplayTagContainer& TagsIn) const
{
	return AllTags.HasAllExact(TagsIn);
}

FAFCueHandle FAFCueHandle::GenerateHandle()
{
	static uint32 HandleIndex = 0;
	HandleIndex++;

	FAFCueHandle NewHandle(HandleIndex);

	return NewHandle;
}

FGAEffectCueParams::FGAEffectCueParams(const FGAEffectContext& InContext, const struct FGAEffectProperty& InProperty)
	: HitResult(InContext.HitResult)
	, Instigator(InContext.Instigator)
	, Causer(InContext.Causer)
	, CueTags(InProperty.GetSpec()->Cues.CueTags)
{};