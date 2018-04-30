// Copyright 2018 Sean Chen. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_BlendLocomotionFour.h"
#include "AnimGraphNode_BlendLocomotionFour.generated.h"

struct FAnimMode_OrientationWarping;
/**
*
*/
UCLASS()
class ORIONANIMATIONEDITOR_API UAnimGraphNode_BlendLocomotionFour : public UAnimGraphNode_Base
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = "Settings")
		FAnimNode_BlendLocomotionFour Node;

	//~ Begin UEdGraphNode Interface.
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	//~ End UEdGraphNode Interface.

	//~ Begin UAnimGraphNode_Base Interface
	virtual FString GetNodeCategory() const override;
	//~ End UAnimGraphNode_Base Interface
	virtual void CustomizePinData(UEdGraphPin* Pin, FName SourcePropertyName, int32 ArrayIndex) const override;
	UAnimGraphNode_BlendLocomotionFour();


};
