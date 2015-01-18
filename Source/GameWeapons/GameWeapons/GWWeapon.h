#pragma once
#include "GWWeapon.generated.h"

USTRUCT(BlueprintType)
struct GAMEWEAPONS_API FGWWeaponHitInfo
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		int8 HitCounter;
	UPROPERTY(BlueprintReadOnly, Category = "HitInfo")
		FVector Origin;
	UPROPERTY(BlueprintReadOnly, Category = "HitInfo")
		FVector HitLocation;
};

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
	class UGWWeaponState* EquipingState;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ActiveState;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* ActionState;

	UPROPERTY(EditAnywhere, Instanced, Category = "States")
	class UGWWeaponState* CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "Target Data")
		TArray<FHitResult> TargetData;

	UPROPERTY()
		APlayerController* WeaponPC;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		FName WeaponSocketName;
	UPROPERTY(EditAnywhere, Category = "Weapon")
		FName TargetingSocketName;
	
	bool bIsWeaponReady;

	UPROPERTY(ReplicatedUsing = OnRep_HitInfo)
		FGWWeaponHitInfo HitInfo;
	UFUNCTION()
		virtual void OnRep_HitInfo();
	virtual void InitializeWeapon();

	virtual void InputPressed();
	virtual void InputReleased();

	virtual void ActionBegin();
	virtual void ActionEnd();
	virtual void BeginCustomization();
	virtual void FinishCustomization();

	inline void SetHitResult(const TArray<FHitResult>& HitResultsIn){ TargetData = HitResultsIn; };

	void GotoState(class UGWWeaponState* NextState);

	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponAttackBegin();

	UFUNCTION(BlueprintImplementableEvent)
		void OnWeaponAttackEnd();

	/*
		Sets impact location from UGWTraceBase::SingleLineTraceSetHitLocation()
		Useful, if you need to display some weapon helper element/effec under
		specific hit location, and update it every frame or every so often ;).
	*/
	void SetHitLocation(FVector StartLocation, FVector ImpactLocation);

	virtual FVector GetTargetingSocketLocation();
	virtual FVector GetWeaponSocketLocation();

	/** AActor overrides */
	virtual void OnRep_Instigator() override;
	/* AActor overrides **/
protected:
	class IIGISocket* SocketInt;
	/*
		Time for how long weapon will be in equiping state (from weapon inventory
		to character hand).
	*/
	float EquipingTime;
public:
	/*
		Sets time, for how long this weapon will be in equiping state.
	*/
	inline void SetEquipingTime(float TimeIn){ EquipingTime = TimeIn; };
};
