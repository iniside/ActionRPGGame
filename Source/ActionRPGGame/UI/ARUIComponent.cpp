// Fill out your copyright notice in the Description page of Project Settings.

#include "ActionRPGGame.h"
#include "ARCharacter.h"
#include "GAAbilitiesComponent.h"
#include "ARUIComponent.h"


// Sets default values for this component's properties
UARUIComponent::UARUIComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UARUIComponent::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (!PC)
	{
		return;
	}
	ENetMode NetMode = GetNetMode();
	//only create UI for clients
	if(NetMode == ENetMode::NM_Client
		|| NetMode == ENetMode::NM_Standalone)
	{
		ARCharacter = Cast<AARCharacter>(PC->GetCharacter());
		if (ARCharacter)
		{
			ARCharacter->GetAbilityComp()->OnTargetAttributeModifed.AddDynamic(this, &UARUIComponent::OnAttributeChanged);
		}
		SAssignNew(MainWidget, SARMainWidget);
		SAssignNew(FloatingText, SARFTContainer).
			PC(PC).
			PoolSize(200).
			OwningComp(this);
		GEngine->GameViewport->AddViewportWidgetContent(MainWidget.ToSharedRef());
		GEngine->GameViewport->AddViewportWidgetContent(FloatingText.ToSharedRef());

		SAssignNew(TargetInfo, SARTargetInfo).PC(PC).UIComp(this);
		GEngine->GameViewport->AddViewportWidgetContent(TargetInfo.ToSharedRef());
	}
	// ...
	
}


// Called every frame
void UARUIComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UARUIComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (!GEngine)
		return;
	if (!GEngine->GameViewport)
		return;
	if (MainWidget.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(MainWidget.ToSharedRef());
		MainWidget.Reset();
	}
	if (FloatingText.IsValid())
	{
		FloatingText->Reset();
		GEngine->GameViewport->RemoveViewportWidgetContent(FloatingText.ToSharedRef());
		FloatingText.Reset();
	}
	if (TargetInfo.IsValid())
	{
		GEngine->GameViewport->RemoveViewportWidgetContent(TargetInfo.ToSharedRef());
		TargetInfo.Reset();
	}
}

void UARUIComponent::EmitCombatText(FVector WorldPos, const FString& InText)
{
	if (FloatingText.IsValid())
	{
		FloatingText->EmitText(WorldPos, InText);
	}
}
void UARUIComponent::OnAttributeChanged(const FAFAttributeChangedData& Data)
{
	if (!Data.Target.IsValid())
		return;
	
	if (Data.Location.Size() > 0)
	{
		FString text = FString::SanitizeFloat(Data.Mod.Value);
		EmitCombatText(Data.Location, text);
	}
	else
	{
		if (AActor* Target = Cast<AActor>(Data.Target.Get()))
		{
			float HalfHeight = Target->GetSimpleCollisionHalfHeight();
			FVector Location = Target->GetActorLocation();
			Location.Z += HalfHeight;
			FString text = FString::SanitizeFloat(Data.Mod.Value);
			EmitCombatText(Location, text);
		}
	}
}

void UARUIComponent::ShowTargetInfo(AActor* InTarget)
{
	if (InTarget)
	{
		if (TargetInfo.IsValid())
		{
			TargetInfo->Show(InTarget);
		}
	}
	else
	{
		if (TargetInfo.IsValid())
		{
			TargetInfo->Hide();
		}
	}
}