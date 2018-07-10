// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilityFramework.h"
#include "MovieScene.h"
#include "GAEffectCueSequence.h"
#include "ActorSequencePlayer.h"
#include "AFCueActor.h"

AAFCueActor::AAFCueActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	StartTime = 0;
	EndTime = 5;
	if (HasAnyFlags(RF_ClassDefaultObject) || GetArchetype() == GetDefault<AAFCueActor>())
	{
		Sequence = ObjectInitializer.CreateDefaultSubobject<UGAEffectCueSequence>(this, "Sequence");
		Sequence->SetFlags(RF_Public | RF_Transactional);
		SequencePlayer = ObjectInitializer.CreateDefaultSubobject<UActorSequencePlayer>(this, "SequencePlayer");
	}
}
void AAFCueActor::PostInitProperties()
{
	UpdateAssetRegistryInfo();
	Super::PostInitProperties();
}

void AAFCueActor::Serialize(FArchive& Ar)
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
void AAFCueActor::UpdateAssetBundleData()
{
	AssetBundleData.Reset();
	UpdateAssetRegistryInfo();

	// By default parse the metadata
	if (UAssetManager::IsValid())
	{
		UAssetManager::Get().InitializeAssetBundlesFromMetadata(this, AssetBundleData);
	}
}

void AAFCueActor::PreSave(const class ITargetPlatform* TargetPlatform)
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
void AAFCueActor::PostLoad()
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
FPrimaryAssetId AAFCueActor::GetPrimaryAssetId() const
{
	FName dupa1 = FPackageName::GetShortFName(GetOutermost()->GetFName());
	
	const AAFCueActor* A = this;
	return FPrimaryAssetId(FPrimaryAssetType("ActorCue"), dupa1);
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

void AAFCueActor::SetAnimation(class UGAEffectCueSequence* InSequence)
{
	Sequence = InSequence;
}
// Called when the game starts or when spawned
void AAFCueActor::BeginPlay()
{
	if (!SequencePlayer)
	{
		SequencePlayer = NewObject<UActorSequencePlayer>(this, UActorSequencePlayer::StaticClass(), "SequencerPlayer");
	}
	SequencePlayer->Initialize(Sequence, PlaybackSettings);
	
	Super::BeginPlay();
	//NativeBeginCue();
}

// Called every frame
void AAFCueActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (SequencePlayer)
	{
		SequencePlayer->Update(DeltaTime);
	}
}
void AAFCueActor::NativeBeginCue(AActor* InstigatorOut, AActor* TargetOut, UObject* Causer,
	const FHitResult& HitInfo, const FGAEffectCueParams& CueParams)
{
	BeginCue(InstigatorOut, TargetOut, Causer, HitInfo);
	if (!SequencePlayer)
	{
		SequencePlayer->Play();
	}
}

void AAFCueActor::NativeOnExecuted()
{
	OnExecuted();
}
void AAFCueActor::NativeOnRemoved()
{
	OnRemoved();
}

void  AAFCueActor::UpdateAssetRegistryInfo()
{
	EffectCueTagSearch = CueTag.GetTagName();
}