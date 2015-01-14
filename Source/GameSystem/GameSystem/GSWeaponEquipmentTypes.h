#pragma once
#include "GameplayTagContainer.h"
#include "GSWeaponEquipmentTypes.generated.h"


USTRUCT()
struct GAMESYSTEM_API FGSWeaponSocketInfo
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	*	Name of socket.
	*/
	UPROPERTY(EditAnywhere)
		FName SocketName;
	/*
		If possible try to attach to this socket first.
	 */
	UPROPERTY(EditAnywhere)
		bool bPreferThisSocket;
	/**
	*	Only actor with these tags, can be attached to this socket.
	*/
	UPROPERTY(EditAnywhere)
		FGameplayTagContainer RequiredTags;

	/**
	*	Animation to play when actor is attached/detached from socket.
	*/
	UPROPERTY(EditAnywhere)
		UAnimMontage* Animation;

	bool bIsSocketAvailable;



	FGSWeaponSocketInfo()
	{
		bIsSocketAvailable = true;
	}
};