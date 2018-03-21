// Fill out your copyright notice in the Description page of Project Settings.

#include "AREnemyHealthBar.h"




void UAREnemyHealthBar::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
/*
	MyTextBlock.Reset();
	HealthBar.Reset();*/
}

//TSharedRef<SWidget> UAREnemyHealthBar::RebuildWidget()
//{
//	MyTextBlock = SNew(STextBlock);
//	MyTextBlock->SetText(FText::FromString("Test Widget"));
//
//	HealthBar = SNew(SProgressBar);
//	
//
//	return MyTextBlock.ToSharedRef();
//}