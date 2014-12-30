#pragma once
#include "GASAbilityAction.h"
#include "GASAbilityActionTrace.generated.h"
/*
	Base class for trace actions.
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew, Within = GASAbility)
class GAMEABILITIES_API UGASAbilityActionTrace : public UGASAbilityAction
{
	GENERATED_UCLASS_BODY()
public:
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
	virtual void Tick(float DeltaSecondsIn) override;

	FVector SingleLineTrace();
	/**
	*	Call it to display cosmetic helpers.
	*/
	virtual void PreExecute() override;

	/**
	 *	Central function to execute current action.
	 */
	virtual void Execute() override;
private:
	/*
		Cached Skeletal mesh component from which we will get Start socket location if needed.
	*/
	UPROPERTY()
		USkeletalMeshComponent* CachedSkeletalMesh;

protected:
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
