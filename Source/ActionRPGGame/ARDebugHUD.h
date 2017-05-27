// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/HUD.h"
#include "ARDebugHUD.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API AARDebugHUD : public AHUD
{
	GENERATED_BODY()
public:
	virtual	void DrawHUD() override;
public:
	UPROPERTY()
		int32 MoveDirection;
	UPROPERTY()
		float StrafeDir;
	UPROPERTY()
		float VelSpeedDot;
	UPROPERTY()
		float AccelX;
	UPROPERTY()
		float AccelY;

	UPROPERTY()
		float VelocityX;
	UPROPERTY()
		float VelocityY;

	UPROPERTY()
		float Angle;
};
