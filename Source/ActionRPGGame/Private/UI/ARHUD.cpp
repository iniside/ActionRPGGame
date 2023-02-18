// Fill out your copyright notice in the Description page of Project Settings.

#include "ARHUD.h"
#include "Engine/GameViewportClient.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ARCharacter.h"
#include "Attributes/ARCharacterAttributes.h"
#include "UI/Inventory/ARUIInventoryComponent.h"

#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "UI/HUD/ARHUDEnemyHealthBar.h"
#include "UI/HUD/ARHUDFloatingCombatText.h"

#include "ARPlayerController.h"

AARHUD::AARHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	UIInventoryComponent = ObjectInitializer.CreateDefaultSubobject<UARUIInventoryComponent>(this, TEXT("UIInventoryComponent"));
	DistanceScaleEnemyBar = 1000.0f;
	HUDFloatingCombatTextClass = UARHUDFloatingCombatText::StaticClass();

}

void AARHUD::BeginPlay()
{
	Super::BeginPlay();
	if (AARPlayerController* PC = Cast<AARPlayerController>(PlayerOwner))
	{
		UIInventoryComponent->CreateInventoryView(PC);

		ARPC = PC;
		ARCharacter = Cast<AARCharacter>(GetOwningPawn());
		if (HUDEnemyHealthBarClass)
		{
			HUDEnemyHealthBar = CreateWidget<UARHUDEnemyHealthBar>(PC, HUDEnemyHealthBarClass);
			HUDEnemyHealthBar->AddToViewport();
			HUDEnemyHealthBar->SetVisibility(ESlateVisibility::Collapsed);
		}

		if (HUDFloatingCombatTextClass)
		{
			FloatingCombatText = CreateWidget<UARHUDFloatingCombatText>(PC, HUDFloatingCombatTextClass);
			FloatingCombatText->AddToViewport();
			FloatingCombatText->SetVisibility(ESlateVisibility::HitTestInvisible);
			FloatingCombatText->PC = ARPC;
			FloatingCombatText->FCTMoveSpeed = FCTMoveSpeed;
			FloatingCombatText->FCTLifeTime = FCTLifeTime;
			FloatingCombatText->Font = FCTFont;
			FloatingCombatText->Init(100);
		}

		if (ARCharacter)
		{
			UAFAbilityComponent* ABComp =  ARCharacter->GetAbilityComp();
			ABComp->OnTargetAttributeModifed.AddDynamic(this, &AARHUD::OnEnemyDamageCaused);
		}
	}
}

void AARHUD::Tick(float InDeltaTime)
{
	Super::Tick(InDeltaTime);

	if (ARCharacter)
	{
		SetEnemyHitResult();
		UpdateEnemyBarInfo();
		FloatingCombatText->Update(InDeltaTime);
	}
	else
	{
		ARCharacter = Cast<AARCharacter>(GetOwningPawn());
	}
}

void AARHUD::ShowHideInventory()
{
	UIInventoryComponent->ShowHideInventory();
}
float AARHUD::ComputeBoundsScreenSize(AActor* InTarget)
{
	ULocalPlayer const* const LP = ARPC->GetLocalPlayer();
	FVector4 ViewOrigin;
	FMatrix ProjMatrix;
	if (LP && LP->ViewportClient)
	{
		// get the projection data
		FSceneViewProjectionData ProjectionData;
		if (LP->GetProjectionData(LP->ViewportClient->Viewport, eSSP_FULL, /*out*/ ProjectionData))
		{
			ViewOrigin = ProjectionData.ViewOrigin;
			ProjMatrix = ProjectionData.ProjectionMatrix;
		}
	}
	FBox Bounds = InTarget->GetComponentsBoundingBox();
	const float Dist = FVector::Dist(InTarget->GetActorLocation(), ViewOrigin);

	// Get projection multiple accounting for view scaling.
	const float ScreenMultiple = FMath::Max(0.5f * ProjMatrix.M[0][0], 0.5f * ProjMatrix.M[1][1]);

	// Calculate screen-space projected radius
	const float ScreenRadius = ScreenMultiple * Bounds.GetExtent().Z / FMath::Max(1.0f, Dist);

	// For clarity, we end up comparing the diameter
	return ScreenRadius * 2.0f;
}
void AARHUD::SetEnemyHitResult()
{
	FVector Start = ARCharacter->GetFollowCamera()->GetComponentLocation();
	FVector Forward = ARCharacter->GetFollowCamera()->GetForwardVector();
	FVector End = (Forward * 10000.0) + Start;
	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(EnemyChannel);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(ARCharacter);
	Params.bTraceComplex = false;
	Params.OwnerTag = TEXT("HUDLineTrace");
	//make async ?
	GetWorld()->LineTraceSingleByChannel(EnemyHitResult, Start, End, CollisionChannel, Params);
}
void AARHUD::UpdateEnemyBarInfo()
{
	if (EnemyHitResult.GetActor())
	{
		AActor* Target = EnemyHitResult.GetActor();
		HUDEnemyHealthBar->SetVisibility(ESlateVisibility::HitTestInvisible);
		FVector ActorLocation = EnemyHitResult.GetActor()->GetActorLocation();
		FVector2D ScreeLoc;

		UGameplayStatics::ProjectWorldToScreen(ARPC, ActorLocation, ScreeLoc, false);

		float Distance = FVector::Dist(ActorLocation, ARCharacter->GetActorLocation());
		Distance = DistanceScaleEnemyBar / Distance;
		Distance = FMath::Clamp<float>(Distance, 0.7, 1);

		float ScreenSize = ComputeBoundsScreenSize(Target);
		FVector Extent = Target->GetComponentsBoundingBox().GetExtent();

		float OffsetY = (Extent.Z * 7) * ScreenSize;
		ScreeLoc.Y = ScreeLoc.Y - OffsetY;

		HUDEnemyHealthBar->SetPositionInViewport(ScreeLoc, true);

		FVector2D NewScale(Distance, Distance);
		HUDEnemyHealthBar->SetRenderScale(NewScale);
		if(IAFAbilityInterface* TargetInt = Cast<IAFAbilityInterface>(Target))
		{
			UARCharacterAttributes* Attributes = TargetInt->GetAttributesTyped<UARCharacterAttributes>();
			if (Attributes)
			{
				float Percent = Attributes->Health.GetCurrentValue() / Attributes->Health.GetFinalValue();
				HUDEnemyHealthBar->UpdateHealth(Percent);
			}
		}

	}
	else
	{
		HUDEnemyHealthBar->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AARHUD::OnEnemyDamageCaused(const FAFAttributeChangedData& InMod)
{
	AActor* Target = EnemyHitResult.GetActor();
	if (Target)
	{
		FVector2D ScreeLoc;

		UGameplayStatics::ProjectWorldToScreen(ARPC, EnemyHitResult.Location, ScreeLoc, false);
		FloatingCombatText->SetInfo(InMod.Mod.Value, ScreeLoc);
	}
	else
	{
		FVector2D ScreeLoc;
		UGameplayStatics::ProjectWorldToScreen(ARPC, InMod.Location, ScreeLoc, false);
		FloatingCombatText->SetInfo(InMod.Mod.Value, ScreeLoc);
	}
}