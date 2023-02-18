// Fill out your copyright notice in the Description page of Project Settings.

#include "../AbilityFramework.h"
#include "../AFAbilityComponent.h"
#include "GAAttributeExtension.h"

void UGAAttributeExtension::OnPreAttributeModify(class UAFAbilityComponent* InComp, const FGAAttribute& Attribute, float InValue)
{
	InComp->NotifyOnPreAttributeModified(Attribute);
}
void UGAAttributeExtension::OnPostAttributeModify(class UAFAbilityComponent* InComp, const FGAAttribute& Attribute, float InValue)
{
	InComp->NotifyOnPostAttributeModified(Attribute);
}