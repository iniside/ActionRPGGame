// Fill out your copyright notice in the Description page of Project Settings.

#include "DWBPFunctionLibrary.h"
#include "DWManager.h"


FDWWWindowHandle UDWBPFunctionLibrary::CreateWindow(const FString& WindowName)
{
	return FDWManager::Get().CreateWindow(WindowName);
}
FDWWWindowHandle UDWBPFunctionLibrary::CreateWindowWithContent(UUserWidget* InWindowContent, const FString& WindowName)
{
	return FDWManager::Get().CreateWindow(InWindowContent->TakeWidget(), WindowName);
}