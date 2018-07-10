#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAGameEffect.h"

#include "LatentActions/AFLatentInterface.h"
#include "GAEffectExtension.generated.h"
/*
	Instanced effect with active event graph, where you can bind events
	on application.
*/
UCLASS(BlueprintType, Blueprintable, EditInLineNew)
class ABILITYFRAMEWORK_API UGAEffectExtension : public UObject, public IAFLatentInterface //this interface is not needed, but NewTask is expting those functions.
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly, Category = "Context")
		FGAEffectContext Context;

	/*Component which actually owns this Effect (Effect Target) */
	UPROPERTY()
		class UAFEffectsComponent* OwningComponent;

	/* Component from this effect has originated (Effect Instigator) */
	UPROPERTY()
		class UAFEffectsComponent* SourceComponent;

	UPROPERTY()
		class AActor* Avatar;

	UPROPERTY()
		TSet<class UAFTaskBase*> ActiveTasks;

	UPROPERTY()
		TMap<FName, class UAFTaskBase*> Tasks;

public:
	UGAEffectExtension(const FObjectInitializer& ObjectInitializer);
	void SetParameters(const FGAEffectContext& ContextIn);
	void BeginEffect();

	/*
		This event is always executed once upon application.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectApplied();
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExecuted();
	/*
		Event executed when effet naturally expires.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExpired();
	/*
		Event executed when this effect is removed by force.
	*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectRemoved();

	void NativeOnEffectApplied();
	void NativeOnEffectExecuted();
	void NativeOnEffectExpired();
	void NativeOnEffectRemoved();

	void NativeOnEffectAppliedCDO();
	void NativeOnEffectExecutedCDO();
	void NativeOnEffectExpiredCDO();
	void NativeOnEffectRemovedCDO();


	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectAppliedCDO() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExecutedCOD() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectExpiredCDO() const;
	UFUNCTION(BlueprintImplementableEvent, Category = "Event Graph")
		void OnEffectRemovedCDO() const;

	virtual UWorld* GetWorld() const override;

	/*  IAFLatentInterface */
	virtual void OnLatentTaskAdded(FName InstanceName, class UAFTaskBase* TaskIn);
	virtual void AddReplicatedTask(class UAFTaskBase* TaskIn);
	virtual void OnLatentTaskRemoved(class UAFTaskBase* TaskIn);

	virtual void OnLatentTaskActivated(class UAFTaskBase* TaskIn);
	virtual void OnLatentTaskDeactivated(class UAFTaskBase* TaskIn);

	virtual class UAFTaskBase* GetCachedLatentAction(FName TaskName);
	/*  IAFLatentInterface */

protected:
	void CleanupTasks();
};
