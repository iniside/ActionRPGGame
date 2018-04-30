// Fill out your copyright notice in the Description page of Project Settings.

#include "../AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "GAAttributeExtension.h"




void UGAAttributeExtension::Initialize(UAFAbilityComponent* InAbilityComponent, const FName& InAttributeName)
{
	AbilityComponent = InAbilityComponent;
	Attribute = FGAAttribute(InAttributeName);
}

void UGAAttributeExtension::OnPreAttributeModify(float InValue)
{
	AbilityComponent->NotifyOnPreAttributeModified(Attribute);
}
void UGAAttributeExtension::OnPostAttributeModify(float InValue)
{
	AbilityComponent->NotifyOnPostAttributeModified(Attribute);
}