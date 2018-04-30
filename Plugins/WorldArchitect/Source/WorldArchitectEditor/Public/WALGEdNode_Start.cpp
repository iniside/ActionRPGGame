// Fill out your copyright notice in the Description page of Project Settings.
#include "WALGEdNode_Start.h"
#include "WALandscapeGraphEditorTypes.h"





void UWALGEdNode_Start::AllocateDefaultPins()
{
	CreatePin(EGPD_Output, UWALandscapeGraphEditorTypes::PinCategory_Start, TEXT("Start"));
}