#pragma once
#include "GameplayTagContainer.h"
#include "GameplayTagAssetInterface.h"

#include "IGAAttributes.h"

#include "GSEffectField.generated.h"
/*
	Need to add attribute component for this actor. We want it to
	have attributes like Health, so we can destroy this actor.
*/
/*
	Very similiar to effect cue, with main difference, that this actor can live indepndetly of
	it's instigator. Instigator just spawn field in world, and from that point on it will
	live defined amount of time (or live indefinetly).

	Also unlike Cue, Fields interact with other actors.
*/

UCLASS(BlueprintType, Blueprintable)
class GAMESYSTEM_API AGSEffectField : public AActor, public IGameplayTagAssetInterface//, public IIGAAttributes
{
	GENERATED_UCLASS_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
	class UGAAttributeComponent* Attributes;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PreInitializeComponents() override;

	virtual void BeginDestroy() override;

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
	 *	Is this field going to live infinite amount of time ?
	 */
	UPROPERTY(BlueprintReadOnly, meta = (ExposeOnSpawn), Category = "Info")
		bool bIsInfinite;
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
	 *	4. If nothing specified, it will use default size defined in blueprint.
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

	virtual void DestroyField();

	/*
		Destroys this field. Call it if you expclitly need to destroy field.
	*/
	UFUNCTION(BlueprintCallable, meta=(FriendlyName="Destroy Field"), Category = "Game System|Effect Field")
		void BP_DestroyField();

	UFUNCTION()
		void OnFieldHit(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
		void OnFieldBeginOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnFieldEndOverlap(class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/*
		Called for every actor in OverlapingActors
	*/
	UFUNCTION(BlueprintImplementableEvent)
		void OnActorHit(AActor* HitActor);

	/*
		If override this don't forget to call Super::OnOtherFieldOverlap
		at the END of function, or manually call BP_OnOtherFieldOverlap
	*/
	virtual void OnOtherFieldOverlap(AGSEffectField* OtherField);

	UFUNCTION(BlueprintImplementableEvent, meta = (FriendlyName="OnOtherFieldOverlap"))
	void BP_OnOtherFieldOverlap(AGSEffectField* OtherField);

	/** IIIGAAttributes overrides */
	//virtual class UGAAttributesBase* GetAttributes() override;
	//virtual class UGAAttributeComponent* GetAttributeComponent() override;
	//virtual void Died() override;
	/* IIIGAAttributes overrides **/

	/** IGameplayTagAssetInterface overrides */
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch = true) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer, bool bCountEmptyAsMatch = true) const override;
	/* IGameplayTagAssetInterface overrides **/
private:
	class IIGSEffectField* FieldInt;
	int32 OverlapingActorCount;

	int32 CurrentLifetime;
};
