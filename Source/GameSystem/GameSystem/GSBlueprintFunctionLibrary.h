#pragma once

#include "GSBlueprintFunctionLibrary.generated.h"
/*
	Some static helper functions, to interact with Attribute system.
*/
UCLASS()
class GAMESYSTEM_API UGSBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Game System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGSEffectField* BeginCreateEffectField(TSubclassOf<class AGSEffectField> EffectFieldClass
		, const FVector& Location
		, class AActor* FieldInstigator);

	UFUNCTION(BlueprintCallable, Category = "Game System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGSEffectField* FinishCreateEffectField(class AGSEffectField* EffectField);


	UFUNCTION(BlueprintCallable, Category = "Game Ability System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGSPersistentCue* BeginSpawnCueActor(TSubclassOf<class AGSPersistentCue> CueActorClass
		, const FVector& Location
		, class AActor* CueInstigator);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System", meta = (UnsafeDuringActorConstruction = "true", BlueprintInternalUseOnly = "true"))
		static class AGSPersistentCue* FinishSpawnCueActor(class AGSPersistentCue* EffectField);
};
