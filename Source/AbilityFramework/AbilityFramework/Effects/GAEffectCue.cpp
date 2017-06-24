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
	if (Period > 0)
	{
		FTimerDelegate del = FTimerDelegate::CreateUObject(this, &AGAEffectCue::NativeOnPeriod);
		FTimerManager& Timer = GetWorld()->GetTimerManager();
		Timer.SetTimer(PeriodTimer, del, Period, true);
	}
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
	const FHitResult& HitInfo)
{
	BeginCue(InstigatorOut, TargetOut, Causer, HitInfo);
	SequencePlayer->Play();
}

void AGAEffectCue::NativeOnPeriod()
{
	OnPeriod();
}