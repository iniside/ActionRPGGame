// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFramework.h"
#include "AFTaskManager.h"

UAFTaskManager* UAFTaskManager::Instance = nullptr;

UAFTaskManager::UAFTaskManager(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
}

UAFTaskManager* UAFTaskManager::Get()
{
	if (Instance)
	{
		return Instance;
	}
	else
	{
		Instance = NewObject<UAFTaskManager>();
		Instance->AddToRoot();
		return Instance;
	}
}