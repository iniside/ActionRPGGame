#pragma once

#include "WALandscapeGraphEditorTypes.generated.h"

UCLASS()
class UWALandscapeGraphEditorTypes : public UObject
{
	GENERATED_UCLASS_BODY()

	static const FName PinCategory_MultipleNodes;
	static const FName PinCategory_SingleNode;
	static const FName PinCategory_MaskInput;
	static const FName PinCategory_FinalInput;

	static const FName PinCategory_InputA;
	static const FName PinCategory_InputB;
	static const FName PinCategory_LerpMask;

	static const FName PinCategory_Output;
	static const FName PinCategory_Start;
};
