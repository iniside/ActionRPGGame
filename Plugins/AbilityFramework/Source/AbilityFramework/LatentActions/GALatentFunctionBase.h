#pragma once
#include "CoreMinimal.h"
#include "Engine/EngineBaseTypes.h"
//#include "Messaging.h"
#include "GALatentFunctionBase.generated.h"

struct FGALatentFunctionTick: public FTickFunction
{
	/**  AActor  that is the target of this tick **/
	class UGALatentFunctionBase* Target;

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
class ABILITYFRAMEWORK_API UGALatentFunctionBase : public UObject
{
	GENERATED_BODY()
	//never access internals of these classes directly. Use messages instead.
protected:
	UPROPERTY()
		UObject* TaskOwner;
	friend struct FGALatentFunctionTick;
	FGALatentFunctionTick TickFunction;

	UGALatentFunctionBase(const FObjectInitializer& ObjectInitializer);
	virtual UWorld* GetWorld() const override;

	//virtual void Tick(float DeltaSecondsIn);
	virtual void TickAction(float DeltaSeconds, ELevelTick TickType, FGALatentFunctionTick& ThisTickFunction) {};
	virtual void Initialize();
	virtual void ReadyForActivation() {};
	virtual void Activate() {};
	virtual void EndTask();
	virtual void BeginDestroy() override;
	
	template <class T>
	FORCEINLINE static T* NewTask(UObject* WorldContextObject, UObject* InTaskOwner, FName InstanceName = FName())
	{
		T* MyObj = NewObject<T>(WorldContextObject);
		MyObj->TaskOwner = InTaskOwner;
		MyObj->Initialize();
		return MyObj;
	}
};
