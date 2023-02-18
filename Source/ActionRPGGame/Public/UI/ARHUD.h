// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ARHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API AARHUD : public AHUD
{
	GENERATED_BODY()
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components|UI")
		class UARUIInventoryComponent* UIInventoryComponent;

	UPROPERTY(EditAnywhere, Category = "Enemy Info")

		TSubclassOf<class UARHUDEnemyHealthBar> HUDEnemyHealthBarClass;
	UPROPERTY(EditAnywhere, Category = "Enemy Info")
		TEnumAsByte<ETraceTypeQuery> EnemyChannel;
	UPROPERTY(EditAnywhere, Category = "Enemy Info")
		float DistanceScaleEnemyBar;

	UPROPERTY(BlueprintReadOnly, Category = "Enemy Info")
		class UARHUDEnemyHealthBar* HUDEnemyHealthBar;

	UPROPERTY(EditAnywhere, Category = "Floating Combat Text")
		TSubclassOf<class UARHUDFloatingCombatText> HUDFloatingCombatTextClass;
	UPROPERTY(BlueprintReadOnly, Category = "Floating Combat Text")
		class UARHUDFloatingCombatText* FloatingCombatText;
	UPROPERTY(EditAnywhere, Category = "Floating Combat Text")
		float FCTMoveSpeed;
	UPROPERTY(EditAnywhere, Category = "Floating Combat Text")
		float FCTLifeTime;
	UPROPERTY(EditAnywhere, Category = "Floating Combat Text")
		FSlateFontInfo FCTFont;

	UPROPERTY()
		class AARCharacter* ARCharacter;
	UPROPERTY()
		class AARPlayerController* ARPC;

	FHitResult EnemyHitResult;
public:
	AARHUD(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	virtual void Tick(float InDeltaTime) override;

	inline UARUIInventoryComponent* GetUIInventory()
	{
		return UIInventoryComponent;
	}


	void ShowHideInventory();

protected:
	float ComputeBoundsScreenSize(AActor* InTarget);
	void SetEnemyHitResult();
	void UpdateEnemyBarInfo();

	UFUNCTION()
		void OnEnemyDamageCaused(const FAFAttributeChangedData& InMod);
};
