// Fill out your copyright notice in the Description page of Project Settings.

#include "../AbilityFramework.h"
#include "MovieScene.h"
#include "GAEffectCueSequence.h"
#include "ActorSequencePlayer.h"
#include "GAEffectCue.h"

AGAEffectCue::AGAEffectCue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	StartTime = 0;
	EndTime = 5;
	if (HasAnyFlags(RF_ClassDefaultObject) || GetArchetype() == GetDefault<AGAEffectCue>())
	{
		Sequence = ObjectInitializer.CreateDefaultSubobject<UGAEffectCueSequence>(this, "Sequence");
		Sequence->SetFlags(RF_Public | RF_Transactional);
		SequencePlayer = ObjectInitializer.CreateDefaultSubobject<UActorSequencePlayer>(this, "SequencePlayer");
	}
}
void AGAEffectCue::PostInitProperties()
{
	Super::PostInitProperties();
}
void AGAEffectCue::SetAnimation(class UGAEffectCueSequence* InSequence)
{
	Sequence = InSequence;
}
// Called when the game starts or when spawned
void AGAEffectCue::BeginPlay()
{
	SequencePlayer->Initialize(Sequence, PlaybackSettings);
	
	Super::BeginPlay();
	//NativeBeginCue();
}

// Called every frame
void AGAEffectCue::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (SequencePlayer)
	{
		SequencePlayer->Update(DeltaTime);
	}
}
void AGAEffectCue::NativeBeginCue(AActor* InstigatorOut, AActor* TargetOut, UObject* Causer,
	const FHitResult& HitInfo, const FGAEffectCueParams& CueParams)
{
	if (CueParams.Period > 0)
	{
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, &AGAEffectCue::NativeOnExecuted);
		FTimerManager& Timer = GetWorld()->GetTimerManager();
		Timer.SetTimer(PeriodTimer, del, CueParams.Period, true);
	}
	BeginCue(InstigatorOut, TargetOut, Causer, HitInfo);
	SequencePlayer->Play();
}

void AGAEffectCue::NativeOnExecuted()
{
	OnExecuted();
}
void AGAEffectCue::NativeOnRemoved()
{
	FTimerManager& Timer = GetWorld()->GetTimerManager();
	Timer.ClearTimer(PeriodTimer);
	SequencePlayer->JumpToPosition(0);
	SequencePlayer->Stop();

	OnRemoved();
}