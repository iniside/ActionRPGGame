// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DWTypes.generated.h"

USTRUCT(BlueprintType)
struct FDWWWindowHandle
{
	GENERATED_BODY()
	TWeakPtr<class SDraggableWindowWidget> Window;
protected:
	uint32 Handle;
public:
	FDWWWindowHandle()
	{};
	FDWWWindowHandle(uint32 InHandle)
		: Handle(InHandle)
	{

	}

	~FDWWWindowHandle()
	{
		Handle = 0;
		Window.Reset();
	}

	static FDWWWindowHandle Make(TSharedPtr<class SDraggableWindowWidget> InWindow);

	friend uint32 GetTypeHash(const FDWWWindowHandle& InHandle)
	{
		return InHandle.Handle;
	}

	const bool operator==(const FDWWWindowHandle& InHandle) const
	{
		return Handle == InHandle.Handle;
	}
};