// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "IGSCue.h"

#include "GSPersistentCue.h"

AGSPersistentCue::AGSPersistentCue(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//tick or timer ?
	//tick for start.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bRunOnAnyThread = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.TickGroup = TG_DuringPhysics;

	CurrentLifeTime = 0;
}

void AGSPersistentCue::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	CurrentLifeTime += DeltaSeconds;
	
	if (CurrentLifeTime > LifeTime)
	{
		DestroyCue();
	}
}

void AGSPersistentCue::InitializeCue()
{
	if (!CueInstigator)
		return;

	CueInt = Cast<IIGSCue>(CueInstigator);

	if (CueInt && LifeTime <= 0)
	{
		//bind Instigator delegates.
		CueInt->GetOnCueActionEnded().AddUObject(this, &AGSPersistentCue::DestroyCue);
		SetOwner(CueInstigator);
	}
	else if (CueInstigator && LifeTime > 0)
	{
		//setup life time.
	}
}


void AGSPersistentCue::DestroyCue()
{
	Destroy();
}