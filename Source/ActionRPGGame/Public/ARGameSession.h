// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameSession.h"

#if LINUX_GAMELIFT
#include "GameLiftServerSDK.h"
#include "GameLiftServerSDKModels.h"
#include "aws/gamelift/server/model/GameProperty.h"
#include "aws/gamelift/server/model/GameSession.h"
#include "aws/gamelift/server/model/Player.h"
#include "aws/gamelift/server/model/PlayerSession.h"
#include "aws/gamelift/server/model/PlayerSessionStatus.h"
#endif

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
