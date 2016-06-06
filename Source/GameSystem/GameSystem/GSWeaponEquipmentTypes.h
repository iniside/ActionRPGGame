#pragma once
#include "GameplayTagContainer.h"
#include "Animation/BlendSpace.h"
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

USTRUCT()
struct GAMESYSTEM_API FGSEquipSocketInfo
{
	GENERATED_USTRUCT_BODY()
public:
	/**
	*	Name of socket.
	*/
	UPROPERTY(EditAnywhere)
		FName SocketName;

	UPROPERTY(EditAnywhere)
		FName InventorySlotName;
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



	FGSEquipSocketInfo()
	{
		bIsSocketAvailable = true;
	}
};

USTRUCT(BlueprintType)
struct GAMESYSTEM_API FGSWeaponAnimation
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UAnimSequence* Animation;
	/*
		It looks like tigly coupled, but in fact you can try to reuse existing blend space and
		see if simple float adjustment will make it useable with current pose.
	*/
	/*
		Blendspace for secondary motion, for this anim pose.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		UBlendSpace* BlendSpace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float AdditiveBlendStrenght;
	/*
		Additive value to Yaw rotation of current pose.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
		float YawRotationAdjustment;

	FGSWeaponAnimation()
	{
		AdditiveBlendStrenght = 0.2f;
		YawRotationAdjustment = 0.0f;
	};
};