#pragma once
#include "GESEffectComponent.generated.h"

/*
	It's not going to look like this. I will probabaly wrap it into struct
	The genral idea we call event, when we apply effect, to give other effects chance to modify newly
	appiled effect.
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGESOnEffectOutgoing, class UGESEffect*, EffectIn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGESOnEffectIncoming, class UGESEffect*, EffectIn);
/*

*/
UCLASS(hidecategories = (Object, LOD, Lighting, Transform, Sockets, TextureStreaming), editinlinenew, meta = (BlueprintSpawnableComponent))
class GAMEEFFECTSYSTEM_API UGESEffectComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()
public:
	/* temporary workaround, hopefully. */
	UPROPERTY(EditAnywhere, Category = "Effect")
		TArray<TSubclassOf<class UGESEffect>> StaticEffectClasses;

	/*
		Static effects are always present on this component and this actor.
		They might not be active. 
		Because of that their indexes in array are also static.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effect")
		TArray<class UGESEffect*> StaticEffects;
	
	/*
		List of periodic effects, contained by this component. This list can change
		during gameplay as other actors/objects might add new effects here, and after their time
		expires, they are removed from array and destroyed.
	*/
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
		TArray<class UGESEffect*> PeriodicEffects;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGESOnEffectOutgoing OnEffectOutgoing;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
		FGESOnEffectIncoming OnEffectIncoming;

	virtual void InitializeComponent() override;

	UFUNCTION(BlueprintCallable, Category = "Game Effect System")
		void AddPeriodicEffect(class UGESEffect* EffectIn);
};
