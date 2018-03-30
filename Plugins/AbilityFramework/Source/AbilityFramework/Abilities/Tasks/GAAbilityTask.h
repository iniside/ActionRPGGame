#pragma once
#include "GameplayTask.h"
#include "../GAAbilityBase.h"
#include "../../AFAbilityComponent.h"

//#include "Messaging.h"
#include "MessageEndpoint.h"
#include "MessageEndpointBuilder.h"

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
class ABILITYFRAMEWORK_API UGAAbilityTask : public UGameplayTask
{
	GENERATED_UCLASS_BODY()
		friend struct FAFAbilityTaskMessageTick;
public:
	/* Ability owning this task */
	TWeakObjectPtr<UGAAbilityBase> Ability;
	/* Ability owning this task */
	TWeakObjectPtr<UAFAbilityComponent> AbilityComponent;
public:
	//virtual UWorld* GetWorld() const override;

	//virtual void Tick(float DeltaSecondsIn);

	virtual void Initialize();
	template <class T>
	static T* NewAbilityTask(UObject* WorldContextObject, FName InTaskName = FName(), FName InstanceName = FName())
	{
		check(WorldContextObject);

		T* MyObj = nullptr;
		UGAAbilityBase* ThisAbility = CastChecked<UGAAbilityBase>(WorldContextObject);
		if (UGAAbilityTask* CachedTask = ThisAbility->GetAbilityTask(InTaskName))
		{
			MyObj = Cast<T>(CachedTask);
		}
		else
		{
			MyObj = NewObject<T>(WorldContextObject);
			ThisAbility->AddAbilityTask(InTaskName, MyObj);
		}
		MyObj->Ability = ThisAbility;
		MyObj->AbilityComponent = ThisAbility->AbilityComponent;
		MyObj->InitTask(*ThisAbility, ThisAbility->GetGameplayTaskDefaultPriority());
		MyObj->InstanceName = InstanceName;
		
		return MyObj;
	}

	template<typename T>
	static bool DelayedFalse()
	{
		return false;
	}

	// this function has been added to make sure AbilityTasks don't use this method
	template <class T>
	FORCEINLINE static T* NewTask(UObject* WorldContextObject, FName InstanceName = FName())
	{
		static_assert(DelayedFalse<T>(), "UAbilityTask::NewTask should never be used. Use NewAbilityTask instead");
	}

	//network overrides:
	int32 GetFunctionCallspace(UFunction* Function, void* Parameters, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;
protected:
	void EndAbilityTask();

	void OnDestroy(bool bInOwnerFinished) override
	{
		ensure(TaskState != EGameplayTaskState::Finished && !IsPendingKill());
		TaskState = EGameplayTaskState::Finished;

		if (TasksComponent.IsValid())
		{
			TasksComponent->OnGameplayTaskDeactivated(*this);
		}

		//MarkPendingKill();
	}
};
