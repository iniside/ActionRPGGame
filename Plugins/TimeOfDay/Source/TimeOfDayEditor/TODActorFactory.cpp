// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "TimeOfDayEditor.h"

#include "UnrealEd.h"
#include "Factories.h"
//
//#include "AssetRegistryModule.h"
//#include "Editor/ContentBrowser/Public/ContentBrowserModule.h"
//#include "Editor/ClassViewer/Public/ClassViewerModule.h"
//#include "Editor/ClassViewer/Public/ClassViewerFilter.h"
//#include "SClassPickerDialog.h"
//#include "MessageLog.h"
//#include "EnumEditorUtils.h"
//#include "StructureEditorUtils.h"
//
//#include "Serialization/Json/Json.h"
//
//#include "Editor/MainFrame/Public/MainFrame.h"
//
//#include "Runtime/JsonUtilities/Public/JsonUtilities.h"

#include "TODActorFactory.h"


UTODActorFactory::UTODActorFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DisplayName = FText::FromString("Time Of Day");
	NewActorClass = ATODActor::StaticClass();
	bUseSurfaceOrientation = false;
}

bool UTODActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (!AssetData.IsValid() || !AssetData.GetClass()->IsChildOf(UTODAsset::StaticClass()))
	{
		//OutErrorMsg = NSLOCTEXT("CanCreateActor", "NoStaticMesh", "A valid static mesh must be specified.");
		return false;
	}

	return true;
}

void UTODActorFactory::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	UTODAsset* TodAsset = CastChecked<UTODAsset>(Asset);
	GEditor->SetActorLabelUnique(NewActor, TodAsset->GetName());

	ATODActor* TodActor = CastChecked<ATODActor>(NewActor);
	TodActor->TODAsset = TodAsset;
	//TodActor->SetActorRotation(FRotator(TodAsset->SunAngle, 0, TodAsset->SunRotation));
	TodActor->CurrentTime = TodAsset->Hour;

	//Setup Sun
	UDirectionalLightComponent* Sun = TodActor->Sun;
	Sun->UnregisterComponent();

	Sun->Intensity = TodAsset->SunIntensityCurve.EditorCurveData.Eval(TodAsset->Hour);
	Sun->LightColor = TodAsset->SunColorCurve.GetLinearColorValue(TodAsset->Hour);
	//Sun->SetRelativeRotation(FRotator(0, TodAsset->SunPosition, 0));
	Sun->SetRelativeRotation(FRotator(TodAsset->SunAngle, TodAsset->SunPosition, TodAsset->SunRotation));
	Sun->RegisterComponent();

	//Setup SunMesh;
	UStaticMeshComponent* SunMesh = TodActor->SunMesh;
	SunMesh->UnregisterComponent();
	SunMesh->SetStaticMesh(TodAsset->SunMesh);
	SunMesh->RegisterComponent();

	//Setup Moon

	//Setup MoonMesh;
	UStaticMeshComponent* MoonMesh = TodActor->MoonMesh;
	MoonMesh->UnregisterComponent();
	MoonMesh->SetStaticMesh(TodAsset->MoonMesh);
	MoonMesh->RegisterComponent();

	//Setup Night Sky
	UStaticMeshComponent* NightSkyMesh = TodActor->NightSky;
	NightSkyMesh->UnregisterComponent();
	NightSkyMesh->SetStaticMesh(TodAsset->NightSkyMesh);
	NightSkyMesh->RegisterComponent();
	NightSkyMesh->SetCastShadow(false);

}

UObject* UTODActorFactory::GetAssetFromActorInstance(AActor* Instance)
{
	//check(Instance->IsA(NewActorClass));
	//AStaticMeshActor* SMA = CastChecked<AStaticMeshActor>(Instance);

	//check(SMA->StaticMeshComponent);
	//return SMA->StaticMeshComponent->StaticMesh;
	
	return nullptr;
}

void UTODActorFactory::PostCreateBlueprint(UObject* Asset, AActor* CDO)
{
	if (Asset != NULL && CDO != NULL)
	{
	//	UStaticMesh* StaticMesh = CastChecked<UStaticMesh>(Asset);
	//	AStaticMeshActor* StaticMeshActor = CastChecked<AStaticMeshActor>(CDO);
	//	UStaticMeshComponent* StaticMeshComponent = StaticMeshActor->StaticMeshComponent;

	//	StaticMeshComponent->StaticMesh = StaticMesh;
	//	StaticMeshComponent->StaticMeshDerivedDataKey = StaticMesh->RenderData->DerivedDataKey;

		UTODAsset* TodAsset = CastChecked<UTODAsset>(Asset);

		ATODActor* TodActor = CastChecked<ATODActor>(CDO);

		TodActor->SetActorRotation(FRotator(TodAsset->SunAngle, 0, TodAsset->SunRotation));
		UDirectionalLightComponent* Sun = TodActor->Sun;
		Sun->UnregisterComponent();

		Sun->Intensity = 4.0f;
		//Sun->SetRelativeRotation(FRotator(0, TodAsset->SunPosition, 0));

		Sun->SetRelativeRotation(FRotator(TodAsset->SunAngle, TodAsset->SunPosition, TodAsset->SunRotation));

		Sun->RegisterComponent();
	}
}
