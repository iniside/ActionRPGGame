#pragma once
#include "IGIPawn.generated.h"


/*
	Implement this interface is you need way to access Pawn/Character/PlayerController from other
	UObject/AActor class, but don't really care about other functionality of this class.
*/
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class GAMEINTERFACES_API UIGIPawn : public UInterface
{
	GENERATED_UINTERFACE_BODY()
};

class IIGIPawn
{
	GENERATED_IINTERFACE_BODY()
public:
	virtual APawn* GetGamePawn() { return nullptr; }
	virtual ACharacter* GetGameCharacter() { return nullptr; }
	virtual AController* GetGameController() { return nullptr; }
	virtual APlayerController* GetGamePlayerController() { return nullptr; }
	virtual UCameraComponent* GetPawnCamera() { return nullptr; }
};