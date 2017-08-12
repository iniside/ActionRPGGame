// Fill out your copyright notice in the Description page of Project Settings.

#include "ARGunAttributes.h"
#include "Net/UnrealNetwork.h"


void UARGunAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UARGunAttributes, Magazine);
}

