// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "ARDebugHUD.h"




void AARDebugHUD::DrawHUD()
{
	int32 X = Canvas->SizeX;
	int32 Y = Canvas->SizeY;

	FVector2D AccPos = FVector2D::ZeroVector;
	AccPos.X = X / 2;
	AccPos.Y = Y / 2;
	DrawText(FString("VelSpeedDot: ") + FString::SanitizeFloat(VelSpeedDot),
		FLinearColor::White, AccPos.X, AccPos.Y, NULL, 1.0F);

	DrawText(FString("MoveDirection: ") + FString::FormatAsNumber(MoveDirection),
		FLinearColor::White, AccPos.X, AccPos.Y-16, NULL, 1.0F);

	DrawText(FString("StrafeDir: ") + FString::SanitizeFloat(StrafeDir),
		FLinearColor::White, AccPos.X, AccPos.Y - 32, NULL, 1.0F);

	DrawText(FString("AccelX: ") + FString::SanitizeFloat(AccelX),
		FLinearColor::White, AccPos.X, AccPos.Y - 48, NULL, 1.0F);

	DrawText(FString("AccelY: ") + FString::SanitizeFloat(AccelY),
		FLinearColor::White, AccPos.X, AccPos.Y - 64, NULL, 1.0F);

	DrawText(FString("VelocityX: ") + FString::SanitizeFloat(VelocityX),
		FLinearColor::White, AccPos.X, AccPos.Y - 80, NULL, 1.0F);

	DrawText(FString("VelocityY: ") + FString::SanitizeFloat(VelocityY),
		FLinearColor::White, AccPos.X, AccPos.Y - 96, NULL, 1.0F);

	DrawText(FString("VelocityY: ") + FString::SanitizeFloat(Angle),
		FLinearColor::White, AccPos.X, AccPos.Y - 112, NULL, 1.0F);
}