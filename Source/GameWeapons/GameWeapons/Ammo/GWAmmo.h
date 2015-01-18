#pragma once
#include "GameplayTagContainer.h"
#include "GWAmmo.generated.h"
/*
	Ammo indicates type of damage dealt by weapon. You can implement it directly in
	Ammo derived class (blueprint or C++), or use it as encapsulation, which will simply 
	pass call to other object.

	Also it's concept appiled pretty much only to ranged weapons. Since mele weapon don't
	use ammo for obvious reasons ;).
*/
UCLASS(BlueprintType, Blueprintable, DefaultToInstanced, EditInLineNew)
class GAMEWEAPONS_API UGWAmmo : public UObject
{
	GENERATED_UCLASS_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
		FGameplayTagContainer OwnedTags;

	UFUNCTION(BlueprintNativeEvent)
		void ApplyDamage(const FHitResult& TargetIn, float DamageIn, APawn* InstigatorIn);
};
