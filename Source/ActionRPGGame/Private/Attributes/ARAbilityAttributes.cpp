#include "ARAbilityAttributes.h"


void UARAbilityAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION(UARAbilityAttributes, CastTime, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UARAbilityAttributes, Cooldown, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UARAbilityAttributes, BaseDamage, COND_OwnerOnly);
}