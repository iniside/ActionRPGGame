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


FGAEffectContext::FGAEffectContext(APawn* InInstigator, UObject* InCauser)
{
	Instigator = InInstigator;
	Causer = InCauser;

	IAFAbilityInterface* ABI = Cast<IAFAbilityInterface>(InInstigator);
	InstigatorAttributes = ABI->GetAttributes();
	InstigatorComp = ABI->GetAbilityComp();
	InstigatorInterface = ABI;
}

void FGAEffectContext::SetTarget(UObject* NewTarget, const FHitResult& InHit)
{
	HitResult = InHit;
	IAFAbilityInterface* ATI = Cast<IAFAbilityInterface>(NewTarget);
	if (!ATI)
	{
		TargetComp.Reset();
		Target.Reset();
		TargetInterface = nullptr;
		TargetAttributes.Reset();
		return;
	}

	TargetComp = ATI->GetAbilityComp();
	Target = NewTarget;
	TargetInterface = ATI;
	TargetAttributes = ATI->GetAttributes();
}
FGAEffectHandle::FGAEffectHandle()
	: Handle(0)
{
	Handle = 0;
}
FGAEffectHandle::FGAEffectHandle(uint32 HandleIn)
	: Handle(HandleIn)
{
}
void FGAEffectHandle::PostScriptConstruct()
{
	Handle = 0;
}
FGAEffectHandle::~FGAEffectHandle()
{
}

/* Executes effect trough provided execution class. */

FGAEffectHandle FGAEffectHandle::GenerateHandle()
{
	static int32 Handle;
	Handle++;
	return FGAEffectHandle(Handle);
}

bool FGAEffectHandle::IsValid() const
{
	return (Handle > 0);// && EffectPtr->Context.IsValid();
}
void FGAEffectHandle::Reset()
{
	Handle = 0;
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

void FGAEffectContext::operator=(const FGAEffectContext& Other)
{
	HitResult			=	Other.HitResult;
	TargetHitLocation	=	Other.TargetHitLocation;
	TargetAttributes = Other.TargetAttributes;
	InstigatorAttributes = Other.InstigatorAttributes;
	Target = Other.Target;
	Causer = Other.Causer;
	Instigator = Other.Instigator;
	Avatar = Other.Avatar;
	TargetComp = Other.TargetComp;
	InstigatorComp = Other.InstigatorComp;
	TargetInterface = Other.TargetInterface;
	InstigatorInterface = Other.InstigatorInterface;
}
FGAEffectContext::FGAEffectContext(const FGAEffectContext& Other)
{
	HitResult = Other.HitResult;
	TargetHitLocation = Other.TargetHitLocation;
	TargetAttributes = Other.TargetAttributes;
	InstigatorAttributes = Other.InstigatorAttributes;
	Target = Other.Target;
	Causer = Other.Causer;
	Instigator = Other.Instigator;
	Avatar = Other.Avatar;
	TargetComp = Other.TargetComp;
	InstigatorComp = Other.InstigatorComp;
	TargetInterface = Other.TargetInterface;
	InstigatorInterface = Other.InstigatorInterface;
}
void FGAEffectContext::Reset()
{
	Target.Reset();
	Causer.Reset();
	Instigator.Reset();
	TargetComp.Reset();
	InstigatorComp.Reset();
	TargetInterface = nullptr;
	InstigatorInterface = nullptr;
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
	, CueTags(InProperty.GetSpecData()->Cues.CueTags)
{};