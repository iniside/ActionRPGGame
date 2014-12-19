#pragma once
#include "GAAttributeComponent.generated.h"

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEATTRIBUTES_API UGAAttributeComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/*
		Temprorary hack. Direct inline spawning of object on ActorComponents doesn't work!
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, meta=(ExposedOnSpawn))
		TSubclassOf<class UGAAttributesBase> DefaultAttributesClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced)
		class UGAAttributesBase* DefaultAttributes;
	virtual void InitializeComponent() override;
};



