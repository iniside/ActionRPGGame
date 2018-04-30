// Fill out your copyright notice in the Description page of Project Settings.

#include "AFDAbilityGiveTrigger.h"
#include "AFAbilityInterface.h"
#include "AFAbilityComponent.h"
#include "Abilities/GAAbilityBase.h"

// Sets default values
AAFDAbilityGiveTrigger::AAFDAbilityGiveTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComp = CreateDefaultSubobject<USceneComponent>(RootCompName);
	RootComponent = RootComp;

	Icon = CreateDefaultSubobject<UBillboardComponent>(IconName);
	Icon->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
	Trigger = CreateDefaultSubobject<UBoxComponent>(TriggerName);
	Trigger->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);

	//Trigger2 = CreateDefaultSubobject<UBoxComponent>(TriggerName);
	//Trigger2->AttachToComponent(RootComp, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AAFDAbilityGiveTrigger::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AAFDAbilityGiveTrigger::BeginOverlap);
	Trigger->OnComponentEndOverlap.AddDynamic(this, &AAFDAbilityGiveTrigger::EndOverlap);
}

// Called every frame
void AAFDAbilityGiveTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAFDAbilityGiveTrigger::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!AbilityConfig.AbilityTag.IsValid())
		return;

	IAFAbilityInterface* AbilityInterface = Cast<IAFAbilityInterface>(OtherActor);
	if (!AbilityInterface)
		return;

	if(!CurrentComponent)
		CurrentComponent = AbilityInterface->GetAbilityComp();

	//FAFOnAbilityReady del = FAFOnAbilityReady::CreateUObject(this, &AAFDAbilityGiveTrigger::OnAbilityReady, AbilityConfig.AbilityTag,
	//	AbilityConfig.InputTag);

	APawn* pawn = Cast<APawn>(OtherActor);
	if(pawn)
	{
		AController* AC = pawn->GetController();
		if(AC)
		{
			TArray<UActorComponent*> Comps = AC->GetComponentsByTag(UAMAbilityManagerComponent::StaticClass(), TEXT("AbilityManager"));
			if(Comps.Num() == 1 && !AbilityManager)
			{
				AbilityManager = Cast<UAMAbilityManagerComponent>(Comps[0]);
			}
			
			if(AbilityManager)
				AbilityManager->NativeEquipAbility(AbilityConfig.AbilityTag, AbilityConfig.Group, AbilityConfig.Slot);
		}
	}

	//if(AbilityManager)
	//{
	//	
	//}
	//CurrentComponent->AddOnAbilityReadyDelegate(AbilityConfig.AbilityTag, del);
	//CurrentComponent->NativeAddAbilityFromTag(AbilityConfig.AbilityTag, nullptr);// , /*Input*/ ShootInput);
}

void AAFDAbilityGiveTrigger::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!AbilityConfig.AbilityTag.IsValid())
		return;
}

void AAFDAbilityGiveTrigger::OnAbilityReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput)
{
	UGAAbilityBase* Ability = Cast<UGAAbilityBase>(CurrentComponent->BP_GetAbilityByTag(InAbilityTag));
	if (GetOwner()->GetNetMode() == ENetMode::NM_Client)
	{
		FAFOnAbilityReady ReadyDelegate = FAFOnAbilityReady::CreateUObject(this, &AAFDAbilityGiveTrigger::OnAbilityInputReady,
			InAbilityTag, InAbilityInput);

		CurrentComponent->SetAbilityToAction(InAbilityTag, InAbilityInput, ReadyDelegate);
	}
	else
	{
		CurrentComponent->SetAbilityToAction(InAbilityTag, InAbilityInput, FAFOnAbilityReady());
	}
}


void AAFDAbilityGiveTrigger::OnAbilityInputReady(TSoftClassPtr<UGAAbilityBase> InAbilityTag, TArray<FGameplayTag> InAbilityInput)
{
	
}