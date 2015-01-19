#pragma once
#include "IGTTrace.h"
#include "Widgets/FCTHudWidget.h"
#include "GSHUD.generated.h"


struct FGSCrosshairDirection
{
	static const int32 Left;
	static const int32 Right;
	static const int32 Top;
	static const int32 Bottom;
	static const int32 Center;
	FGSCrosshairDirection()
	{};
};

/*
	Hud need to retrieve information about:
	1. Current weapon socket.
	2. Ability Socket.
	3. What is currently active.
	4. Display crosshair according to above information.
*/
UCLASS()
class GAMESYSTEM_API AGSHUD : public AHUD, public IIGTTrace
{
	GENERATED_UCLASS_BODY()
public:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	/**
	 *	Trace to where crosshair will be displayed. It's approximated, since crosshair
	 *	is traced against world and special Crosshair collision box around pawn.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Crosshair")
	class UGTTraceBase* CrossHairTrace;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crosshair")
	class UTexture2D* CrossHairTexture;

	UPROPERTY()
		FCanvasIcon Crosshair[5];
	UPROPERTY(EditAnywhere, Category = "Crosshair")
		float CrosshairScale;
	UPROPERTY(EditAnywhere, Category = "Crosshair")
		float CrossSpread;
	void DrawCrosshair();

	/**
	 *	Where hit will really hit. Crosshair is traced against world and
	 *	special collision box around pawn. this trace will ignore crosshair box.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Crosshair")
	class UGTTraceBase* BulletHitTrace;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Instanced, Category = "Floating Combat Text")
		UFCTHudWidget* FCTWidget;

	virtual void DrawHUD() override;

	/** IIGTTrace Begin */
	virtual FVector GetSocketLocation(FName SocketNameIn);

	virtual APawn* GetPawn() { return GetOwningPawn(); };

	virtual APlayerController* GetPC() { return GetOwningPlayerController(); };

	virtual FVector GetLocation() { return GetOwningPawn()->GetActorLocation(); };

	virtual void SetTargetData(const TArray<FHitResult>& DataIn) {};
	/** IIGTTrace End */

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Crosshair")
		FVector2D CrossHairPosition;

	UPROPERTY(BlueprintReadOnly, Category = "Crosshair")
		FVector2D HitPosition;

	UPROPERTY()
		class AGSCharacter* OwnChar;

	class IIGTSocket* iSocket;

	bool bInitialized;

	float CurrentSpread;
	void SetCurrentSpread(float CurrentSpreadIn);
	class UGSItemWeaponRangedInfo* CurrentLeftWeapon;
};
