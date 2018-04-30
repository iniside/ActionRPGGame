#pragma once
#include "GameplayTask.h"
#include "GAAbilityBase.h"
#include "AFAbilityComponent.h"

#include "LatentActions/AFTaskBase.h"

#include "GAAbilityTask.generated.h"
/*
	AbilityActions are generic (preferably C++) defined actions, which then can be added to ability and
	the should be activated from ability. 
	Then can perform tasks, like spawn tagetting helpers (splines, circles), spawn actors, 
	gather targeting data etc.

	Should they be activated automatically after ability is initialized, (it'e ability enterted in
	active state, which means it's ready to be fired and display helpers, but did not yet received input,
	or should designer in blueprint decide when to launch actions ?).
*/

UCLASS(BlueprintType, Blueprintable, Within=GAAbilityBase)
class ABILITYFRAMEWORK_API UGAAbilityTask : public UAFTaskBase
{
	GENERATED_BODY()
		friend struct FAFAbilityTaskMessageTick;
public:
	uint8 bIsReplicated : 1;
	/* Ability owning this task */
	TWeakObjectPtr<UGAAbilityBase> Ability;
	/* Ability owning this task */
	TWeakObjectPtr<UAFAbilityComponent> AbilityComponent;
public:

	template <class T>
	static T* NewAbilityTask(UGAAbilityBase* WorldContextObject, FName InTaskName = FName(), FName InstanceName = FName())
	{
		check(WorldContextObject);

		T* MyObj = nullptr;
		UGAAbilityBase* ThisAbility = CastChecked<UGAAbilityBase>(WorldContextObject);
		MyObj = NewTask<T, UGAAbilityBase>(WorldContextObject, WorldContextObject, InTaskName);

		MyObj->Ability = ThisAbility;
		MyObj->AbilityComponent = ThisAbility->AbilityComponent;
		
		return MyObj;
	}
	UGAAbilityTask(const FObjectInitializer& ObjectInitializer);
	bool IsReplicated()
	{
		return bIsReplicated;
	}

protected:
	bool IsClient()
	{
		APawn* POwner = Ability->POwner;
		if (POwner->GetNetMode() == ENetMode::NM_Client)
		{
			return true;
		}
		return false;
	}

	bool IsServer()
	{
		APawn* POwner = Ability->POwner;
		if (POwner->GetNetMode() == ENetMode::NM_DedicatedServer
			|| POwner->GetNetMode() == ENetMode::NM_ListenServer)
		{
			return true;
		}
		return false;
	}
	bool IsServerOrStandalone()
	{
		APawn* POwner = Ability->POwner;
		if (POwner->GetNetMode() == ENetMode::NM_DedicatedServer
			|| POwner->GetNetMode() == ENetMode::NM_ListenServer
			|| POwner->GetNetMode() == ENetMode::NM_Standalone)
		{
			return true;
		}
		return false;
	}

	bool IsAuthority()
	{
		APawn* POwner = Ability->POwner;
		if (POwner->Role >= ENetRole::ROLE_Authority)
		{
			return true;
		}
		return false;
	}
};
