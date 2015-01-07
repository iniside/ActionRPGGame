#pragma once
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"
#include "GSEffectField.generated.h"

/*
	Very similiar to effect cue, with main difference, that this actor can live indepndetly of
	it's instigator. Instigator just spawn field in world, and from that point on it will
	live defined amount of time (or live indefinetly).

	Also unlike Cue, Fields interact with other actors.
*/

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSEffectField : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_UCLASS_BODY()


	virtual void Tick(float DeltaSeconds) override;

	virtual void PreInitializeComponents() override;

	/*
		Needs way to dynamically update life time, when CueInstigator life time changes.
		For example channeled ability has extended duration during casting,
		but post this actor created.
	*/
	/**
	 *	Maximum amount of time, this effect field will persist in level after spwaning.
	 *  if <= 0 it will exist as long as CueInstigator performing action.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Info")
		float LifeTime;

	/**
	 *	Maximum amount of actors this field can affect at the same time.
	 *	First come, first serve.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Info")
		int32 MaximumTargets;

	/**
	 *	Size of field if:
	 *	1. Box - normal rules applu.
	 *	2. Sphere - first non-zero value is considered radius (best to put it in X);
	 *	3. Cylinder - X Radius, Y - Height.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Info")
		FVector FieldSize;

	/**
	 *	I have these tags.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Tags")
		FGameplayTagContainer MyTags;

	/**
	 *	I require these tags on other actor, to affect it.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn), Category = "Tags")
		FGameplayTagContainer RequiredTags;

	/*
		Who Instigated this cue.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Info")
		AActor* EffectFieldInstigator;
protected:
	/*
		Filtered list of actors. Only actor which really can be affected are in this array.
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Overlaped Actors")
		TArray<AActor*> OverlapingActors;
public:
	/*
		Initialize effect, but does not play it. Use it setup your properties.
	*/
	UFUNCTION(BlueprintCallable, Category = "Game System|Effect Field") //temporary for testing.
		virtual void InitializeEffectField();

	UFUNCTION()
		void OnFieldHit(class AActor* OtherActor, class UPrimitiveComponent* HitComponent, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnFieldBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnFieldEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*
		Called for every actor in OverlapingActors
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnActorHit(AActor* HitActor);

	void OnOtherFieldOverlap(AGSEffectField* OtherField);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName="OnOtherFieldOverlap"))
	void BP_OnOtherFieldOverlap(AGSEffectField* OtherField);

	/** IGameplayTagAssetInterface overrides */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;

	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch = true) const override;
	
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch = true) const override;
	/** IGameplayTagAssetInterface overrides */
private:
	class IIGSEffectField* FieldInt;
	int32 OverlapingActorCount;

	int32 CurrentLifetime;
};
