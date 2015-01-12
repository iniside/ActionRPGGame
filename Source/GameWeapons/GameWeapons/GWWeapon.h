#pragma once
#include "GWWeapon.generated.h"

/*
	Base class for all weapons.
	Mele and ranged weapons need separate classes (at least).
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEWEAPONS_API AGWWeapon : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* RootMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
		USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ActiveState;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ActionState;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Target Data")
		TArray<FHitResult> TargetData;

	virtual void InitializeWeapon();

	virtual void InputPressed();
	virtual void InputReleased();

	virtual void BeginCustomization();
	virtual void FinishCustomization();

	inline void SetHitResult(const TArray<FHitResult>& HitResultsIn){ TargetData = HitResultsIn; };

	void GotoState(class UGWWeaponState* NextState);

	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponAttackBegin();

	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponAttackEnd();
};
