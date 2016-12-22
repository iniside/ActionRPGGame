#pragma once
#include "GameplayTask.h"
#include "../GAAbilityBase.h"
#include "../GAAbilitiesComponent.h"
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
class GAMEABILITIES_API UGAAbilityTask : public UGameplayTask
{
	GENERATED_UCLASS_BODY()
public:
	/* Ability owning this task */
	TWeakObjectPtr<UGAAbilityBase> Ability;
	/* Ability owning this task */
	TWeakObjectPtr<UGAAbilitiesComponent> AbilityComponent;
public:
	//virtual UWorld* GetWorld() const override;

	//virtual void Tick(float DeltaSecondsIn);

	virtual void Initialize();
	template <class T>
	static T* NewAbilityTask(UObject* WorldContextObject, FName InstanceName = FName())
	{
		check(WorldContextObject);

		T* MyObj = NewObject<T>(WorldContextObject);
		UGAAbilityBase* ThisAbility = CastChecked<UGAAbilityBase>(WorldContextObject);
		MyObj->InitTask(*ThisAbility, ThisAbility->GetGameplayTaskDefaultPriority());
		MyObj->InstanceName = InstanceName;
		MyObj->AbilityComponent = ThisAbility->AbilityComponent;
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
};
