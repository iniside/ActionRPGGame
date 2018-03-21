// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Notifications/SProgressBar.h"
#include "AREnemyHealthBar.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPGGAME_API UAREnemyHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	//virtual TSharedRef<SWidget> RebuildWidget() override;
protected:
//	TSharedPtr<STextBlock> MyTextBlock;
//	TSharedPtr<SProgressBar> HealthBar;
};
