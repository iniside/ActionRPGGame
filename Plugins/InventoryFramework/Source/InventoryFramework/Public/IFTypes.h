// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Json.h"
#include "Policies/CondensedJsonPrintPolicy.h"
#include "Serialization/JsonTypes.h"
#include "Serialization/JsonReader.h"
#include "Policies/PrettyJsonPrintPolicy.h"
#include "Serialization/JsonSerializer.h"
#include "IFTypes.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FIFItemEvent, uint8, uint8, class UIFItemBase*);
DECLARE_MULTICAST_DELEGATE(FIFOnInventoryChanged);

USTRUCT(BlueprintType)
struct INVENTORYFRAMEWORK_API FIFItemData
{
	GENERATED_BODY()
public:
	UPROPERTY()
		class UIFItemBase* Item;

	UPROPERTY(BlueprintReadOnly)
		uint8 Index;
};


USTRUCT(BlueprintType)
struct INVENTORYFRAMEWORK_API FIFSlotAcceptedClasses
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UIFItemBase>> AcceptedClasses;

};

struct INVENTORYFRAMEWORK_API FIFJsonSerializer
{
	static bool ConvertScalarJsonValueToUProperty(TSharedPtr<FJsonValue> JsonValue, UProperty* Property, void* OutValue);
	static bool JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, const UStruct* StructDefinition, void* OutStruct);
	static bool JsonValueToUProperty(TSharedPtr<FJsonValue> JsonValue, UProperty* Property, void* OutValue);
	static void JsonObjectToUObject(TSharedPtr<FJsonObject> Object, UObject*& OutObject, UObject* Outer);
	static bool JsonAttributesToUObject(const TMap< FString, TSharedPtr<FJsonValue> >& JsonAttributes, const UStruct* StructDefinition, void* OutStruct);
};


/**
 * 
 */
class INVENTORYFRAMEWORK_API IFTypes
{
public:
	IFTypes();
	~IFTypes();
};
