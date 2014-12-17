#pragma once
#include "GAAttributeComponent.generated.h"

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEATTRIBUTES_API UGAAttributeComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, meta=(ExposedOnSpawn))
		TSubclassOf<class UGAAttributesBase> DefaultAttributesClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		class UGAAttributesBase* DefaultAttributes;


	virtual void InitializeComponent() override;
};



