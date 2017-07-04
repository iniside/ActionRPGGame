// Fill out your copyright notice in the Description page of Project Settings.

#include "ARCharacterAttributes.h"
#include "Net/UnrealNetwork.h"



void UARCharacterAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UARCharacterAttributes, Health);
	DOREPLIFETIME(UARCharacterAttributes, Shield);
	DOREPLIFETIME(UARCharacterAttributes, Armor);
	DOREPLIFETIME(UARCharacterAttributes, Energy);
	DOREPLIFETIME(UARCharacterAttributes, Stamina);

}