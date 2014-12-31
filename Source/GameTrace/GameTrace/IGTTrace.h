#pragma once
#include "IGTTrace.generated.h"



UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMETRACE_API UIGTTrace : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGTTrace
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual FVector GetSocketLocation(FName SocketNameIn){ return FVector::ZeroVector; };

	virtual APawn* GetPawn() { return nullptr; };

	virtual APlayerController* GetPC() { return nullptr; };

	virtual FVector GetLocation() { return FVector::ZeroVector; };

	virtual void SetTargetData(const TArray<FHitResult>& DataIn) {};

	virtual TArray<FHitResult>& GetTargetData()
	{
		
		return ret;
	};
private:
	//because I need something for GetTargetData().
	TArray<FHitResult> ret;
};