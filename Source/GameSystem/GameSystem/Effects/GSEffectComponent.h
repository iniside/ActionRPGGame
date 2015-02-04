#pragma once
#include "Effects/GAEffectComponent.h"
#include "GSEffectComponent.generated.h"

UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMESYSTEM_API UGSEffectComponent : public UGAEffectComponent
{
	GENERATED_BODY()
public:
	UGSEffectComponent(const FObjectInitializer& ObjectInitializer);
};
