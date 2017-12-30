// Fill out your copyright notice in the Description page of Project Settings.

#include "DWBPFunctionLibrary.h"
#include "DWManager.h"


FDWWWindowHandle UDWBPFunctionLibrary::CreateWindow()
{
	return FDWManager::Get().CreateWindow();
}
FDWWWindowHandle UDWBPFunctionLibrary::CreateWindowWithContent(UUserWidget* InWindowContent)
{
	return FDWManager::Get().CreateWindow(InWindowContent->TakeWidget());
}