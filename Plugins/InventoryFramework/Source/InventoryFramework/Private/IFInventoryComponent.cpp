// Fill out your copyright notice in the Description page of Project Settings.

#include "IFInventoryComponent.h"

#include "GameFramework/Actor.h"
#include "Engine/AssetManager.h"

#include "IFItemBase.h"
#include "IFItemActorBase.h"
#include "IFInventoryInterface.h"
#include "IFEquipmentComponent.h"
#include "Net/UnrealNetwork.h"
#include "Engine/ActorChannel.h"

#include "Json.h"
#include "JsonObjectConverter.h"

#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"

// Sets default values for this component's properties
UIFInventoryComponent::UIFInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	bAutoActivate = true;
	bAutoRegister = true;

	MaxSlots = 8;
	AvailableSlots = 8;
	// ...
}

void UIFInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

// Called when the game starts
void UIFInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	uint8 Counter = 0;
	for (uint8 Idx = 0; Idx < MaxSlots; Idx++)
	{
		FIFItemData NewItem;
		NewItem.Item = nullptr;
		NewItem.Index = Idx;
		Counter++;
		InventoryItems.Add(NewItem);
	}
	
	/*
		Further steps
		2. Load Properties from external data source (JSON);
	*/
	// ...
	
}

// Called every frame
void UIFInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<uint8> UIFInventoryComponent::GetLocalItemIdxs(TSubclassOf<UIFItemBase> ItemClass)
{
	TArray<uint8> Idxs;
	for (uint8 Idx = 0; Idx < InventoryItems.Num(); Idx++)
	{
		if (InventoryItems[Idx].Item && InventoryItems[Idx].Item->IsA(ItemClass))
		{
			Idxs.Add(Idx);
		}
	}

	return Idxs;
	//return Inventory.GetLocalItemIdxs(ItemClass);
}

bool UIFInventoryComponent::AcceptItem(UIFItemBase* Item, uint8 InLocaLIndex)
{
	bool bAccept = false;

	if (AcceptedClasses.Num() == 0)
	{
		bAccept = true;
	}
	else
	{
		for (TSubclassOf<UIFItemBase>& ItemClass : AcceptedClasses)
		{
			bAccept = false;
			if (Item->IsA(ItemClass))
			{
				bAccept = true;
				break; //no reason to check further.
			}
		}
	}

	if (bAccept)
	{
		if (AcceptedSlotClasses.Num() == 0)
		{
			bAccept = true;
		}
		else
		{
			FIFSlotAcceptedClasses AC = AcceptedSlotClasses[InLocaLIndex];
			if (AC.AcceptedClasses.Num() == 0)
			{
				bAccept = true;
			}
			else
			{
				for (TSubclassOf<UIFItemBase>& ItemClass : AC.AcceptedClasses)
				{
					bAccept = false;
					if (Item->IsA(ItemClass))
					{
						bAccept = true;
						break; //no reason to check further.
					}
				}
			}
		}
		
	}
	return bAccept;
}

void UIFInventoryComponent::MoveItemInInventory(uint8 NewLocalPostion, uint8 OldLocalPositin)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}

	const FIFItemData& NewSlot = GetSlot(NewLocalPostion);
	const FIFItemData& OldSlot = GetSlot(OldLocalPositin);
}

void UIFInventoryComponent::ServerMoveItemInInventory_Implementation(uint8 NewNetPostion, uint8 OldNetPositin)
{
}
bool UIFInventoryComponent::ServerMoveItemInInventory_Validate(uint8 NewNetPostion, uint8 OldNetPositin)
{
	return true;
}

void UIFInventoryComponent::AddAllItemsFromActor(class AIFItemActorBase* Source)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerAddAllItemsFromActor(Source);
		return;
	}
	TArray<TSoftClassPtr<UIFItemBase>> Items = Source->GetAllItems();
	for (const TSoftClassPtr<UIFItemBase> Item : Items)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoadedFreeSlot, Item);

		Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
	}
}
void UIFInventoryComponent::ServerAddAllItemsFromActor_Implementation(class AIFItemActorBase* Source)
{
	typedef TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriter;
	typedef TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriterFactory;

	TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> Vals;

	TSharedPtr<FJsonValueString> dupa = MakeShareable(new FJsonValueString("dupa string"));
	Vals.Add(dupa);

	Obj->SetArrayField("TestData", Vals);
	TSharedPtr<FJsonValueString> dupaData = MakeShareable(new FJsonValueString("dupa asdasd string"));
	Obj->SetField("DupaData", dupaData);
	
	FString OutputString;
	TSharedRef< FPrettyJsonStringWriter > Writer = FPrettyJsonStringWriterFactory::Create(&OutputString);
	check(FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer));

	FString asd;



	//ClientSendJsonData(OutputString);
	TArray<TSoftClassPtr<UIFItemBase>> Items = Source->GetAllItems();
	for (const TSoftClassPtr<UIFItemBase> Item : Items)
	{
		FStreamableManager& Manager = UAssetManager::GetStreamableManager();

		FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoadedFreeSlot, Item);

		Manager.RequestAsyncLoad(Item.ToSoftObjectPath(), Delegate);
	}
}
bool UIFInventoryComponent::ServerAddAllItemsFromActor_Validate(class AIFItemActorBase* Sourcex)
{
	return true;
}



void UIFInventoryComponent::AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		return;
	}
}
void UIFInventoryComponent::BP_AddAllItemsFromActor(class AIFItemActorBase* Source)
{
	AddAllItemsFromActor(Source);
}


void UIFInventoryComponent::ServerAddItemFromClass_Implementation(FSoftObjectPath Item, uint8 InNetIndex)
{
	FStreamableManager& Manager =  UAssetManager::GetStreamableManager();
	TSoftClassPtr<UIFItemBase> It(Item);
	FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &UIFInventoryComponent::OnItemLoaded, It, InNetIndex);

	UE_LOG(LogTemp, Warning, TEXT("ServerAddItemFromClass_Implementation %s"), *Item.ToString());
	Manager.RequestAsyncLoad(Item, Delegate);
}
bool UIFInventoryComponent::ServerAddItemFromClass_Validate(FSoftObjectPath Item, uint8 InNetIndex)
{
	return true;
}
void UIFInventoryComponent::BP_AddItemFromClass(TSoftClassPtr<class UIFItemBase> Item, uint8 InLocalIndex)
{
	AddItemFromClass(Item, InLocalIndex);
}


void UIFInventoryComponent::AddItemFromEquipment(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex)
{

}

void UIFInventoryComponent::ServerAddItemFromEquipment_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex)
{

}
bool UIFInventoryComponent::ServerAddItemFromEquipment_Validate(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex)
{
	return true;
}
void UIFInventoryComponent::ClientAddItemFromEquipment_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex)
{

}

void UIFInventoryComponent::AddItemFromEquipmentAnySlot(class UIFEquipmentComponent* Source, uint8 SourceIndex)
{
	if (GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		UIFItemBase* Item = Source->GetItem(SourceIndex);
		if (!Item)
			return;
		ServerAddItemFromEquipmentAnySlot(Source, SourceIndex);
		return;
	}
}
void UIFInventoryComponent::ServerAddItemFromEquipmentAnySlot_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex)
{
	UIFItemBase* Item = Source->GetItem(SourceIndex);
	if (!Item)
		return;

	uint8 FreeSlot = 0;
	for (uint8 Idx = 0; Idx < InventoryItems.Num(); Idx++)
	{
		if (InventoryItems[Idx].Item == nullptr)
		{
			FreeSlot = Idx;
			break;
		}
	}

	InventoryItems[FreeSlot].Item = DuplicateObject<UIFItemBase>(Item, this);
	ClientAddItemFromEquipmentAnySlot(Source, SourceIndex, FreeSlot);
}
bool UIFInventoryComponent::ServerAddItemFromEquipmentAnySlot_Validate(class UIFEquipmentComponent* Source, uint8 SourceIndex)
{
	return true;
}
void UIFInventoryComponent::ClientAddItemFromEquipmentAnySlot_Implementation(class UIFEquipmentComponent* Source, uint8 SourceIndex, uint8 InventoryIndex)
{
	UIFItemBase* Item = Source->GetItem(SourceIndex);

	InventoryItems[InventoryIndex].Item = DuplicateObject<UIFItemBase>(Item, this);
	Source->RemoveFromEquipment(SourceIndex);
}


void UIFInventoryComponent::RemoveItem(uint8 InIndex)
{
	if(GetOwnerRole() < ENetRole::ROLE_Authority)
	{
		ServerRemoveItem(InIndex);
		return;
	}
	//remove from backend
	if (InventoryItems[InIndex].Item)
		InventoryItems[InIndex].Item->MarkPendingKill();

	InventoryItems[InIndex].Item = nullptr;
	OnServerItemRemoved(InIndex);
	ClientRemoveItem(InIndex);
}
void UIFInventoryComponent::ServerRemoveItem_Implementation(uint8 InIndex)
{
	if (InventoryItems[InIndex].Item)
		InventoryItems[InIndex].Item->MarkPendingKill();

	InventoryItems[InIndex].Item = nullptr;
	OnServerItemRemoved(InIndex);
	ClientRemoveItem(InIndex);
}
bool UIFInventoryComponent::ServerRemoveItem_Validate(uint8 InIndex)
{
	return true;
}
void UIFInventoryComponent::ClientRemoveItem_Implementation(uint8 InIndex)
{
	if (InventoryItems[InIndex].Item)
		InventoryItems[InIndex].Item->MarkPendingKill();

	InventoryItems[InIndex].Item = nullptr;
	OnItemRemoved(InIndex);
}
void UIFInventoryComponent::OnItemLoadedFreeSlot(TSoftClassPtr<class UIFItemBase> InItem)
{
	TSubclassOf<UIFItemBase> ItemClass = InItem.Get();

	FIFItemData Item;
	uint8 FreeIndex = 0;

	for (uint8 Idx = 0; Idx < InventoryItems.Num(); Idx++)
	{
		if (!InventoryItems[Idx].Item)
		{
			FreeIndex = Idx;
			break;
		}
	}
	Item.Index = FreeIndex;
	Item.Item = NewObject<UIFItemBase>(this, ItemClass);
	//Inventory.AddItemToFreeSlot(Item);
	
	
	TSharedPtr<FJsonObject> Obj = MakeShareable(new FJsonObject());
	FJsonObjectConverter::UStructToJsonObject(FIFItemData::StaticStruct(), &Item, Obj.ToSharedRef(), 0, 0);

	FakeBackend.Add(Obj);

	InventoryItems[Item.Index] = Item;


	typedef TJsonWriter< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriter;
	typedef TJsonWriterFactory< TCHAR, TPrettyJsonPrintPolicy<TCHAR> > FPrettyJsonStringWriterFactory;

	FString OutputString;
	TSharedRef< FPrettyJsonStringWriter > Writer = FPrettyJsonStringWriterFactory::Create(&OutputString);
	check(FJsonSerializer::Serialize(Obj.ToSharedRef(), Writer));

	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.Unload(InItem.ToSoftObjectPath());
	ClientSendJsonData(OutputString);
}
void UIFInventoryComponent::OnItemLoaded(TSoftClassPtr<class UIFItemBase> InItem, uint8 InNetIndex)
{
	TSubclassOf<UIFItemBase> ItemClass = InItem.Get();

	UIFItemBase* Item = NewObject<UIFItemBase>(this, ItemClass);

	FStreamableManager& Manager = UAssetManager::GetStreamableManager();
	Manager.Unload(InItem.ToSoftObjectPath());
}

void UIFInventoryComponent::ClientSendJsonData_Implementation(const FString& Data)
{
	TSharedRef< TJsonReader<> > Reader = TJsonReaderFactory<>::Create(Data);

	TSharedPtr<FJsonObject> Object = MakeShareable(new FJsonObject());

	bool bSuccessful = FJsonSerializer::Deserialize(Reader, Object);

	TArray< TSharedPtr<FJsonValue> > Array;
	bool bSuccessful2 = FJsonSerializer::Deserialize(Reader, Array);

	FIFItemData Item;
	FJsonObjectConverter::JsonObjectToUStruct(Object.ToSharedRef(), Item.StaticStruct(), &Item, 0, 0);

	if (Item.Item)
	{
		InventoryItems[Item.Index] = Item;
		OnItemAddedEvent.Broadcast(Item.Index, Item.Index, Item.Item);
		OnItemUpdatedEvent.Broadcast(Item.Index, Item.Index, Item.Item);
	}
	
}