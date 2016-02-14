// Fill out your copyright notice in the Description page of Project Settings.

#include "GameAbilities.h"
#include "GASAbilitiesComponent.h"
#include "GASAbilityBase.h"
#include "GASInputOverride.h"




void UGASInputOverride::NativeInputPressed(int32 Index)
{
	OnInputPressed(Index);
}
void UGASInputOverride::NativeInputReleased(int32 Index)
{
	OnInputReleased(Index);
}