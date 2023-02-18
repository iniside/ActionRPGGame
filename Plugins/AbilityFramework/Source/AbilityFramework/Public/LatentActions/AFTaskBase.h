#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
#include "AFLatentInterface.h"

#include "AFTaskBase.generated.h"

struct FGALatentFunctionTick: public FTickFunction
{
	/**  AActor  that is the target of this tick **/
	class UAFTaskBase* Target;

	/**
	* Abstract function actually execute the tick.
	* @param DeltaTime - frame time to advance, in seconds
	* @param TickType - kind of tick for this frame
	* @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
	* @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completetion of this task until certain child tasks are complete.
	**/
	virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
	/** Abstract function to describe this tick. Used to print messages about illegal cycles in the dependency graph **/
	virtual FString DiagnosticMessage() override;
};

UCLASS(meta = (ExposedAsyncProxy = "true"))
class ABILITYFRAMEWORK_API UAFTaskBase : public UObject
{
	GENERATED_BODY()
	//never access internals of these classes directly. Use messages instead.
protected:
	enum class EState : uint8
	{
		Waiting,
		Active,
		Finished
	};

	EState TaskState;
	bool bReplicated;

	UPROPERTY()
		UObject* TaskOwner;
	friend struct FGALatentFunctionTick;
	FGALatentFunctionTick TickFunction;

	UAFTaskBase(const FObjectInitializer& ObjectInitializer);
	virtual UWorld* GetWorld() const override;

	//virtual void Tick(float DeltaSecondsIn);
	virtual void TickTask(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction) {};
	virtual void Initialize();
public:
	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"), Category = "Latent Action")
	virtual void ReadyForActivation();
	virtual void EndTask();
protected:
	virtual void Activate() {};
	
	virtual void BeginDestroy() override;
	virtual void OnTaskEnded() {};
public:
	/* Replication */
	bool IsNameStableForNetworking() const override;

	bool IsSupportedForNetworking() const override
	{
		return bReplicated;
	}
	void SetNetAddressable();

protected:

	//use template to avoid using interface
	template<typename TaskType, typename OwnerType>
	static TaskType* NewTask(UObject* WorldContextObject, OwnerType* InTaskOwner, FName InstanceName = FName())
	{
		TaskType* MyObj = nullptr;
		
		if (!InstanceName.IsNone())
		{
			MyObj = Cast<TaskType>(InTaskOwner->GetCachedLatentAction(InstanceName));
			if (!MyObj)
			{
				MyObj = NewObject<TaskType>(WorldContextObject);

				InTaskOwner->OnLatentTaskAdded(InstanceName, MyObj);
			}
		}
		else
		{
			MyObj = NewObject<TaskType>(WorldContextObject);

			InTaskOwner->OnLatentTaskAdded(InstanceName, MyObj);
		}
		if (MyObj->bReplicated)
		{
			InTaskOwner->AddReplicatedTask(MyObj);
		}
		MyObj->TaskOwner = InTaskOwner;
		
		return MyObj;
	}
};
