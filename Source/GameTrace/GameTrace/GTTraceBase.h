#pragma once
#include "GameTrace.h"
#include "GTGlobalTypes.h"
#include "GTTraceBase.generated.h"
/*
	This actually could be moved into separate module ? I know i might need something
	very similiar inside Effectsm but on the other hand the Within part makes
	it very easy to use in first place.

	Draft Plan:
	1. Move classes to separate module.
	2. Make them dependand on single object like UTraceObject
	3. If any other object or actor want to use these tracing object, they would need to 
	have instance of UTraceObject.
	4. Another possibility is to make, Interafce, which would be implemented by object/actor, which
	want to make use of this module.


	5. Or I can just as well copy/paste these files to different modules. Not that good, since well
	to much updateding!.

	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew)
class GAMETRACE_API UGTTraceBase : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	virtual UWorld* GetWorld() const override;

	virtual void BeginDestroy() override;
	/*
		We will need tick for updating position of targeting helper actors.
		Tick it along currently active ability.

		Otherwise not really useful.
	*/
	/*
		I have yet to determine best way of aquiring skeletal mesh, from which I want to get socket
		location.

		I might want to implement interface, or possibly scrap it all togather and leave it game specifc
		implementation.

		While ability can be traced only from one socket at time, there might be multiple
		suiteable skeletal meshes present (weapons, multiple character parts), from which to select socket.
	*/

	UPROPERTY(EditAnywhere, Category = "Configuration")
		float Range;

	/**
	*	Should trace start in socket ? If false it will start from center of screen.
	*/
	UPROPERTY(EditAnywhere, Category = "Configuration")
		bool bTraceFromSocket;
	/**
	 *	Name of socket or bone, from which to start trace.
	 */
	UPROPERTY(EditAnywhere, Category = "Configuration")
		FName StartLocation;

	UPROPERTY(EditAnywhere, Category = "Configuration")
		TArray<TEnumAsByte<ECollisionChannel> > ObjectsToTrace;

	UPROPERTY(EditAnywhere, Category = "Configuration")
		bool bIgnoreSelf;

	UPROPERTY(EditAnywhere, Category = "Configuration")
		bool bDrawDebug;
	/*
		TODO::
		1. Add filtering by tags.
		2. Add filtering by class.

		You should be use both at the same time, one or neither.
		They will allow to return prefiltered list of targets, which meets requriments,
		like owned tags, or are on specific type.
	*/
	
	/**/
	virtual void Tick(float DeltaSecondsIn);

	void SingleLineTrace();

	void SingleLineTraceSetHitLocation();

	FVector GetSingHitLocation();

	virtual void Initialize();

	/**
	*	Call it to display cosmetic helpers.
	*/
	virtual void PreExecute();

	/**
	 *	Central function to execute current action.
	 */
	virtual void Execute();

	virtual void PostExecute();

	/**
	 *	Modify area of effect for area traces, by precentage.
	 */
	virtual void ModifyExtendsByPrecentage(float PrcentageIn);

	UFUNCTION(BlueprintCallable, Category = "Game Ability System")
		void BP_ModiyExtendsByPrcentage(float PrecentageIn);

	/**
	*	Modify Range of Trace, by precentage value.
	*/
	virtual void ModifyRangeByPrecentage(float PrcentageIn);

	virtual void SetTraceRange(float PrcentageIn);

	UFUNCTION(BlueprintCallable, Category = "Game Trace")
		virtual FVector GetHelperScale();
private:
	/*
		Cached Skeletal mesh component from which we will get Start socket location if needed.
	*/
	UPROPERTY()
		USkeletalMeshComponent* CachedSkeletalMesh;

protected:
	class IIGTTrace* TraceInterface;

	/*
		Initialized after Actor::BeginPlay(),
		to avoid not needed 
	*/
	FCollisionObjectQueryParams CollisionObjectParams;

	/*
		Helper functions for tracing.
	*/
	/**
	 *	Get pawn aim direction.
	 */
	FVector GetPawnCameraAim();

	/**
	*	Get PlayerController aim direction.
	*/
	FVector GetPCCameraAim();

	/**
	*	Get star trace location from socket on skeletal mesh.
	*/
	FVector GetStartLocationFromSocket();
	/**
	 *	Get start location from pawn camera location.
	 */
	FVector GetPawnCameraDamageStartLocation(const FVector& AimDir);
	/**
	*	Base single line trace.
	*/
	FHitResult SingleLineRangedTrace(const FVector& StartTrace, const FVector& EndTrace);
};
