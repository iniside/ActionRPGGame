#include "ARCharacterAttributes.h"
#include "Net/UnrealNetwork.h"


UARCharacterAttributes::UARCharacterAttributes(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UARCharacterAttributes::GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UARCharacterAttributes, Health);
	DOREPLIFETIME(UARCharacterAttributes, Shield);
	DOREPLIFETIME(UARCharacterAttributes, Armor);
	DOREPLIFETIME(UARCharacterAttributes, Energy);
	DOREPLIFETIME(UARCharacterAttributes, Stamina);
	DOREPLIFETIME(UARCharacterAttributes, Ammo);
	DOREPLIFETIME(UARCharacterAttributes, MachineGunAmmo);
}

void UARCharacterAttributes::OnRep_Health()
{
	if (Health.CurrentValue)
	{

	}
}