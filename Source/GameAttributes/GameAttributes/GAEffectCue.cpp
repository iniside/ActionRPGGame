// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAttributes.h"
#include "GAEffectCue.h"


AGAEffectCue::AGAEffectCue(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;
}

void AGAEffectCue::Destroyed()
{
	Super::Destroyed();
}