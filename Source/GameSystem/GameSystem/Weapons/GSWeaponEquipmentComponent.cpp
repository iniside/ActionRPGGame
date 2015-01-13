// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "GameSystem.h"

#include "../GSCharacter.h"

#include "GSWeaponEquipmentComponent.h"

UGSWeaponEquipmentComponent::UGSWeaponEquipmentComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}
void UGSWeaponEquipmentComponent::InitializeComponent()
{
	EquipInt = Cast<IIGSEquipment>(GetOwner());
	Super::InitializeComponent();
}
void UGSWeaponEquipmentComponent::AttachActorTo(AActor* ActorIn, FName& AttachedSocketNameOut, const TArray<FGSWeaponSocketInfo>& PossibleSockets)
{
	if (EquipInt)
	{
		//iterate over all available sockets. possibly, create temporary array
		//which contain prefiltered list of only available sockets ?
		for (FGSWeaponSocketInfo& pawnSocket : AttachmentSockets)
		{
			//check if there is something attached to socket.
			if (pawnSocket.bIsSocketAvailable)
			{
				//if not iterate over sockets, to which this weapon can be possibly attached.
				for (const FGSWeaponSocketInfo& weapSocket : PossibleSockets)
				{
					//if any of this socket matches available sockets attach weapon.
					if (pawnSocket.SocketName == weapSocket.SocketName)
					{
						AttachedSocketNameOut = weapSocket.SocketName;
						pawnSocket.bIsSocketAvailable = false;
						EquipInt->AttachActor(ActorIn, weapSocket.SocketName);

						return;
					}
				}
			}
		}
	}
}
