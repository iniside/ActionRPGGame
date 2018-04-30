#pragma once

#include "WALandscapeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWAOnHeightmapReady, const TArray<uint16>&, OutHeighmap);

UCLASS(Blueprintable)
class WORLDARCHITECTEDITOR_API UWALandscapeNode : public UObject
{
	GENERATED_BODY()

public:
	UWALandscapeNode();
	virtual ~UWALandscapeNode();
	UPROPERTY()
		bool bDirty;

	//////////////////////////////////////////////////////////////////////////
	// uproperties

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GenericGraphNode")
		FString CustomNodeTitle;

	UPROPERTY(BlueprintReadOnly, Category = "GenericGraphNode")
		UWALandscapeNode* MaskNode;

#if WITH_EDITOR
	UPROPERTY(EditDefaultsOnly, Category = "GenericGraphNode")
	FLinearColor BackgroundColor;
#endif
	UPROPERTY()
		FWAOnHeightmapReady OnHeightmapReady;
	//////////////////////////////////////////////////////////////////////////
	// ufunctions
	UFUNCTION(BlueprintCallable, Category = "GenericGraphNode")
	FString GetNodeTitle();

	virtual void ExecuteNode(const TArray<uint16>& InHeightData) {};

	//////////////////////////////////////////////////////////////////////////
	class UWALandscapeGraph* GetGraph();


	virtual TArray<uint16> GenerateHeightmap();
};
