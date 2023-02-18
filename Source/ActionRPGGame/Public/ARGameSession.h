// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"

#include "ARGameSession.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API AARGameSession : public AGameSession
{
	GENERATED_BODY()
	
	virtual FString ApproveLogin(const FString& Options) override;
	virtual void UnregisterPlayer(const APlayerController* ExitingPlayer) override;
	
};
