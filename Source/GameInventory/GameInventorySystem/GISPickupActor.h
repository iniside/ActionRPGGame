#pragma once
#include "GameplayTags.h"
#include "GameplayTagContainer.h"
#include "IGIInteractable.h"
#include "GISPickupActor.generated.h"
/*
	It's not actor that we pick and physically put it inventory.
	
	It's actor which contain list of items to loot. It's actor only because we need to place
	something to interact on level and Actor lends it self pretty well to it.

	Aside from very specific cases you SHOULD NEVER PUT SPAWNED ACTORS IN INVENTORY!
	Try to always store them as classes.
*/
UCLASS(BlueprintType, Blueprintable)
class GAMEINVENTORYSYSTEM_API AGISPickupActor : public AActor, public IIGIInteractable
{
	GENERATED_BODY()
public:
	//Instanced
	UPROPERTY(ReplicatedUsing=OnRep_ItemToLoot, EditAnywhere, BlueprintReadWrite, Instanced)
		TArray<class UGISItemData*> ItemToLoot;
	UFUNCTION()
		void OnRep_ItemToLoot();
	/*
		Is this actor currently looted ?

		Only one player at time, can interact with this actor.
	*/
	UPROPERTY()
		bool bIsCurrentlyBeingLooted;

	/*
		Set pointer to inventory component, which will interact with this
		loot container. This should allow to loot it properly.
	*/
	UPROPERTY(BlueprintReadOnly)
		TWeakObjectPtr<class UGISInventoryBaseComponent> InteractingInventory;
public:
	AGISPickupActor(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	
	/** IIGIInteractable - BEGIN */
	//UFUNCTION(BlueprintCallable, Category = "Game Interfaces | Interact")
	virtual void Interact(AActor* InteractingActor) override;
	/* IIGIInteractable - END **/
	
	void StartLooting(AActor* WhoPicks);

	UFUNCTION(Server, Reliable, WithValidation)
		void ServerStartLooting(AActor* WhoPicks);
	virtual void ServerStartLooting_Implementation(AActor* WhoPicks);
	virtual bool ServerStartLooting_Validate(AActor* WhoPicks);
	/*
		Should be called when item count in array reaches zero, or life time of actor experies.
		It will actually call DestroyActor(), but it can be used for cleanup/spawning cosmetic effects
		before actually destroying.
	*/
	virtual void DestroyPickupActor();

	virtual void GetLifetimeReplicatedProps(TArray< class FLifetimeProperty > & OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel *Channel, class FOutBunch *Bunch, FReplicationFlags *RepFlags) override;
	virtual void GetSubobjectsWithStableNamesForNetworking(TArray<UObject*>& Objs) override;

	/** Called on the actor when a new subobject is dynamically created via replication */
	virtual void OnSubobjectCreatedFromReplication(UObject *NewSubobject) override;

	/** Called on the actor when a new subobject is dynamically created via replication */
	virtual void OnSubobjectDestroyFromReplication(UObject *NewSubobject) override;
	/*
		Intention to work is as fallows:
		1. User interact with this actor.
		2. Loot Window is displayed.
		3. Loot window is populated with items from ItemToLoot (or other array).
		4. When user click on item, it's added to inventory.

		Problems:
		1. Technically player do not own loot actor, so client can't call RPC functions on it.
		2. That could be solved, by redirecting all adding/removing functions trough inventory component.
		or taking ownership of loot actor.
		3. So widget, which will show loot window, must have reference to inventory component.
		4. What if there are multiple componenets ? We must determine, which component, wants to interact with 
		item container.
		5. Then, when player click on item to loot, inventory component will make RPC call to server,
		and on server component will check item container item list, if that item is on server it will be looted.

	*/
	//loot widget ? Hmp. Build In Inventory Component, to display list of items here ?
	//possibly to complicated. We just need list to items to loot with basic interaction..
	//we are not going to put back items to this container.
	//loot widget it is then!
};
