// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUDWidget.h"




void UARHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UPanelWidget* RootWidget = Cast<UPanelWidget>(GetRootWidget());
	MainGrid = WidgetTree->ConstructWidget<UUniformGridPanel>(UUniformGridPanel::StaticClass(), TEXT("MainGrid"));
	RootWidget->AddChild(MainGrid);
	// Bind delegates here.
}