// Fill out your copyright notice in the Description page of Project Settings.

#include "IFTypes.h"
#include "JsonObjectConverter.h"


bool FIFJsonSerializer::ConvertScalarJsonValueToUProperty(TSharedPtr<FJsonValue> JsonValue, UProperty* Property, void* OutValue)
{
	if (UEnumProperty* EnumProperty = Cast<UEnumProperty>(Property))
	{
		if (JsonValue->Type == EJson::String)
		{
			// see if we were passed a string for the enum
			const UEnum* Enum = EnumProperty->GetEnum();
			check(Enum);
			FString StrValue = JsonValue->AsString();
			int64 IntValue = Enum->GetValueByName(FName(*StrValue));
			if (IntValue == INDEX_NONE)
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"), *Enum->CppType, *StrValue, *Property->GetNameCPP());
				return false;
			}
			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutValue, IntValue);
		}
		else
		{
			// AsNumber will log an error for completely inappropriate types (then give us a default)
			EnumProperty->GetUnderlyingProperty()->SetIntPropertyValue(OutValue, (int64)JsonValue->AsNumber());
		}
	}
	else if (UNumericProperty *NumericProperty = Cast<UNumericProperty>(Property))
	{
		if (NumericProperty->IsEnum() && JsonValue->Type == EJson::String)
		{
			// see if we were passed a string for the enum
			const UEnum* Enum = NumericProperty->GetIntPropertyEnum();
			check(Enum); // should be assured by IsEnum()
			FString StrValue = JsonValue->AsString();
			int64 IntValue = Enum->GetValueByName(FName(*StrValue));
			if (IntValue == INDEX_NONE)
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import enum %s from string value %s for property %s"), *Enum->CppType, *StrValue, *Property->GetNameCPP());
				return false;
			}
			NumericProperty->SetIntPropertyValue(OutValue, IntValue);
		}
		else if (NumericProperty->IsFloatingPoint())
		{
			// AsNumber will log an error for completely inappropriate types (then give us a default)
			NumericProperty->SetFloatingPointPropertyValue(OutValue, JsonValue->AsNumber());
		}
		else if (NumericProperty->IsInteger())
		{
			if (JsonValue->Type == EJson::String)
			{
				// parse string -> int64 ourselves so we don't lose any precision going through AsNumber (aka double)
				NumericProperty->SetIntPropertyValue(OutValue, FCString::Atoi64(*JsonValue->AsString()));
			}
			else
			{
				// AsNumber will log an error for completely inappropriate types (then give us a default)
				NumericProperty->SetIntPropertyValue(OutValue, (int64)JsonValue->AsNumber());
			}
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to set numeric property type %s for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
			return false;
		}
	}
	else if (UBoolProperty *BoolProperty = Cast<UBoolProperty>(Property))
	{
		// AsBool will log an error for completely inappropriate types (then give us a default)
		BoolProperty->SetPropertyValue(OutValue, JsonValue->AsBool());
	}
	else if (UStrProperty *StringProperty = Cast<UStrProperty>(Property))
	{
		// AsString will log an error for completely inappropriate types (then give us a default)
		StringProperty->SetPropertyValue(OutValue, JsonValue->AsString());
	}
	else if (UArrayProperty *ArrayProperty = Cast<UArrayProperty>(Property))
	{
		if (JsonValue->Type == EJson::Array)
		{
			TArray< TSharedPtr<FJsonValue> > ArrayValue = JsonValue->AsArray();
			int32 ArrLen = ArrayValue.Num();

			// make the output array size match
			FScriptArrayHelper Helper(ArrayProperty, OutValue);
			Helper.Resize(ArrLen);

			// set the property values
			for (int32 i = 0; i<ArrLen; ++i)
			{
				const TSharedPtr<FJsonValue>& ArrayValueItem = ArrayValue[i];
				if (ArrayValueItem.IsValid() && !ArrayValueItem->IsNull())
				{
					if (!JsonValueToUProperty(ArrayValueItem, ArrayProperty->Inner, Helper.GetRawPtr(i)))
					{
						UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to deserialize array element [%d] for property %s"), i, *Property->GetNameCPP());
						return false;
					}
				}
			}
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TArray from non-array JSON key for property %s"), *Property->GetNameCPP());
			return false;
		}
	}
	else if (UMapProperty* MapProperty = Cast<UMapProperty>(Property))
	{
		if (JsonValue->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> ObjectValue = JsonValue->AsObject();

			FScriptMapHelper Helper(MapProperty, OutValue);

			// set the property values
			for (const auto& Entry : ObjectValue->Values)
			{
				if (Entry.Value.IsValid() && !Entry.Value->IsNull())
				{
					int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();

					TSharedPtr<FJsonValueString> TempKeyValue = MakeShareable(new FJsonValueString(Entry.Key));

					const bool bKeySuccess = JsonValueToUProperty(TempKeyValue, MapProperty->KeyProp, Helper.GetKeyPtr(NewIndex));
					const bool bValueSuccess = JsonValueToUProperty(Entry.Value, MapProperty->ValueProp, Helper.GetValuePtr(NewIndex));

					if (!(bKeySuccess && bValueSuccess))
					{
						UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to deserialize map element [key: %s] for property %s"), *Entry.Key, *Property->GetNameCPP());
						return false;
					}
				}
			}

			Helper.Rehash();
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TMap from non-object JSON key for property %s"), *Property->GetNameCPP());
			return false;
		}
	}
	else if (USetProperty* SetProperty = Cast<USetProperty>(Property))
	{
		if (JsonValue->Type == EJson::Array)
		{
			TArray< TSharedPtr<FJsonValue> > ArrayValue = JsonValue->AsArray();
			int32 ArrLen = ArrayValue.Num();

			FScriptSetHelper Helper(SetProperty, OutValue);

			// set the property values
			for (int32 i = 0; i < ArrLen; ++i)
			{
				const TSharedPtr<FJsonValue>& ArrayValueItem = ArrayValue[i];
				if (ArrayValueItem.IsValid() && !ArrayValueItem->IsNull())
				{
					int32 NewIndex = Helper.AddDefaultValue_Invalid_NeedsRehash();
					if (!JsonValueToUProperty(ArrayValueItem, SetProperty->ElementProp, Helper.GetElementPtr(NewIndex)))
					{
						UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to deserialize set element [%d] for property %s"), i, *Property->GetNameCPP());
						return false;
					}
				}
			}

			Helper.Rehash();
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TSet from non-array JSON key for property %s"), *Property->GetNameCPP());
			return false;
		}
	}
	else if (UTextProperty* TextProperty = Cast<UTextProperty>(Property))
	{
		if (JsonValue->Type == EJson::String)
		{
			// assume this string is already localized, so import as invariant
			TextProperty->SetPropertyValue(OutValue, FText::FromString(JsonValue->AsString()));
		}
		else if (JsonValue->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
			check(Obj.IsValid()); // should not fail if Type == EJson::Object

								  // import the subvalue as a culture invariant string
			FText Text;
			if (!FJsonObjectConverter::GetTextFromObject(Obj.ToSharedRef(), Text))
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import FText from JSON object with invalid keys for property %s"), *Property->GetNameCPP());
				return false;
			}
			TextProperty->SetPropertyValue(OutValue, Text);
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import FText from JSON that was neither string nor object for property %s"), *Property->GetNameCPP());
			return false;
		}
	}
	else if (UStructProperty *StructProperty = Cast<UStructProperty>(Property))
	{
		static const FName NAME_DateTime(TEXT("DateTime"));
		static const FName NAME_Color(TEXT("Color"));
		static const FName NAME_LinearColor(TEXT("LinearColor"));
		if (JsonValue->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
			check(Obj.IsValid()); // should not fail if Type == EJson::Object
			if (!JsonObjectToUStruct(Obj.ToSharedRef(), StructProperty->Struct, OutValue))
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - FJsonObjectConverter::JsonObjectToUStruct failed for property %s"), *Property->GetNameCPP());
				return false;
			}
		}
		else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_LinearColor)
		{
			FLinearColor& ColorOut = *(FLinearColor*)OutValue;
			FString ColorString = JsonValue->AsString();

			FColor IntermediateColor;
			IntermediateColor = FColor::FromHex(ColorString);

			ColorOut = IntermediateColor;
		}
		else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_Color)
		{
			FColor& ColorOut = *(FColor*)OutValue;
			FString ColorString = JsonValue->AsString();

			ColorOut = FColor::FromHex(ColorString);
		}
		else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetFName() == NAME_DateTime)
		{
			FString DateString = JsonValue->AsString();
			FDateTime& DateTimeOut = *(FDateTime*)OutValue;
			if (DateString == TEXT("min"))
			{
				// min representable value for our date struct. Actual date may vary by platform (this is used for sorting)
				DateTimeOut = FDateTime::MinValue();
			}
			else if (DateString == TEXT("max"))
			{
				// max representable value for our date struct. Actual date may vary by platform (this is used for sorting)
				DateTimeOut = FDateTime::MaxValue();
			}
			else if (DateString == TEXT("now"))
			{
				// this value's not really meaningful from json serialization (since we don't know timezone) but handle it anyway since we're handling the other keywords
				DateTimeOut = FDateTime::UtcNow();
			}
			else if (FDateTime::ParseIso8601(*DateString, DateTimeOut))
			{
				// ok
			}
			else if (FDateTime::Parse(DateString, DateTimeOut))
			{
				// ok
			}
			else
			{
				UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable to import FDateTime for property %s"), *Property->GetNameCPP());
				return false;
			}
		}
		else if (JsonValue->Type == EJson::String && StructProperty->Struct->GetCppStructOps() && StructProperty->Struct->GetCppStructOps()->HasImportTextItem())
		{
			UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();

			FString ImportTextString = JsonValue->AsString();
			const TCHAR* ImportTextPtr = *ImportTextString;
			if (!TheCppStructOps->ImportTextItem(ImportTextPtr, OutValue, PPF_None, nullptr, (FOutputDevice*)GWarn))
			{
				// Fall back to trying the tagged property approach if custom ImportTextItem couldn't get it done
				Property->ImportText(ImportTextPtr, OutValue, PPF_None, nullptr);
			}
		}
		else if (JsonValue->Type == EJson::String)
		{
			FString ImportTextString = JsonValue->AsString();
			const TCHAR* ImportTextPtr = *ImportTextString;
			Property->ImportText(ImportTextPtr, OutValue, PPF_None, nullptr);
		}
		else
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import UStruct from non-object JSON key for property %s"), *Property->GetNameCPP());
			return false;
		}
	}
	else
	{
		// Default to expect a string for everything else
		if (Property->ImportText(*JsonValue->AsString(), OutValue, 0, NULL) == NULL)
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Unable import property type %s from string value for property %s"), *Property->GetClass()->GetName(), *Property->GetNameCPP());
			return false;
		}
	}

	return true;
}

bool FIFJsonSerializer::JsonValueToUProperty(TSharedPtr<FJsonValue> JsonValue, UProperty* Property, void* OutValue)
{
	if (!JsonValue.IsValid())
	{
		UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Invalid value JSON key"));
		return false;
	}

	bool bArrayOrSetProperty = Property->IsA<UArrayProperty>() || Property->IsA<USetProperty>();
	bool bJsonArray = JsonValue->Type == EJson::Array;

	if (!bJsonArray)
	{
		if (bArrayOrSetProperty)
		{
			UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - Attempted to import TArray from non-array JSON key"));
			return false;
		}

		if (Property->ArrayDim != 1)
		{
			UE_LOG(LogJson, Warning, TEXT("Ignoring excess properties when deserializing %s"), *Property->GetName());
		}

		return ConvertScalarJsonValueToUProperty(JsonValue, Property, OutValue);
	}

	// In practice, the ArrayDim == 1 check ought to be redundant, since nested arrays of UPropertys are not supported
	if (bArrayOrSetProperty && Property->ArrayDim == 1)
	{
		// Read into TArray
		return ConvertScalarJsonValueToUProperty(JsonValue, Property, OutValue);
	}

	// We're deserializing a JSON array
	const auto& ArrayValue = JsonValue->AsArray();
	if (Property->ArrayDim < ArrayValue.Num())
	{
		UE_LOG(LogJson, Warning, TEXT("Ignoring excess properties when deserializing %s"), *Property->GetName());
	}

	// Read into native array
	int ItemsToRead = FMath::Clamp(ArrayValue.Num(), 0, Property->ArrayDim);
	for (int Index = 0; Index != ItemsToRead; ++Index)
	{
		if (!ConvertScalarJsonValueToUProperty(ArrayValue[Index], Property, (char*)OutValue + Index * Property->ElementSize))
		{
			return false;
		}
	}
	return true;
}

bool FIFJsonSerializer::JsonAttributesToUObject(const TMap< FString, TSharedPtr<FJsonValue> >& JsonAttributes, const UStruct* StructDefinition, void* OutStruct)
{
	// iterate over the struct properties
	for (TFieldIterator<UProperty> PropIt(StructDefinition); PropIt; ++PropIt)
	{
		UProperty* Property = *PropIt;
		FString PropertyName = Property->GetName();

		// find a json value matching this property name
		TSharedPtr<FJsonValue> JsonValue;
		for (auto It = JsonAttributes.CreateConstIterator(); It; ++It)
		{
			// use case insensitive search sincd FName may change caseing strangely on us
			if (PropertyName.Equals(It.Key(), ESearchCase::IgnoreCase))
			{
				JsonValue = It.Value();
				break;
			}
		}
		if (!JsonValue.IsValid() || JsonValue->IsNull())
		{
			// we allow values to not be found since this mirrors the typical UObject mantra that all the fields are optional when deserializing
			continue;
		}

		void* Value = Property->ContainerPtrToValuePtr<uint8>(OutStruct);
		if (!JsonValueToUProperty(JsonValue, Property, Value))
		{
			UE_LOG(LogJson, Error, TEXT("JsonObjectToUStruct - Unable to parse %s.%s from JSON"), *StructDefinition->GetName(), *PropertyName);
			return false;
		}
	}

	return true;
}

bool FIFJsonSerializer::JsonObjectToUStruct(const TSharedRef<FJsonObject>& JsonObject, const UStruct* StructDefinition, void* OutStruct)
{
	return JsonAttributesToUObject(JsonObject->Values, StructDefinition, OutStruct);
}

void FIFJsonSerializer::JsonObjectToUObject(TSharedPtr<FJsonObject> Object, UObject*& OutObject, UObject* Outer)
{
	FString objClassStr = Object->GetStringField("objectClass");

	FSoftClassPath path(objClassStr);

	UClass* itemCls = Cast<UClass>(path.TryLoad());
	if (itemCls)
	{
		OutObject = NewObject<UObject>(Outer, itemCls);
		for (TFieldIterator<UProperty> PropIt(itemCls); PropIt; ++PropIt)
		{
			UProperty* Property = *PropIt;
			FString PropertyName = Property->GetName();
			TSharedPtr<FJsonValue> JsonValue;
			for (auto It = Object->Values.CreateConstIterator(); It; ++It)
			{
				// use case insensitive search sincd FName may change caseing strangely on us
				if (PropertyName.Equals(It.Key(), ESearchCase::IgnoreCase))
				{
					JsonValue = It.Value();
					break;
				}
			}
			if (!JsonValue.IsValid())
				continue;

			if(UObjectProperty* ObjectProp = Cast<UObjectProperty>(Property))
			{
				const TSharedPtr<FJsonObject>* Obj;
				JsonValue->TryGetObject(Obj);

				if (Obj)
				{
					TSharedPtr<FJsonObject> Objj = *Obj;

					//TSharedPtr<FJsonObject> itemField = Objj->GetObjectField("item");
					FString objClassStr2 = Objj->GetStringField("objectClass");
					if (objClassStr2.Len() > 0)
					{
						FSoftClassPath pathLocal(objClassStr2);
						UClass* itemClsLocal = Cast<UClass>(pathLocal.TryLoad());
						UObject* Out = nullptr;
						JsonObjectToUObject(Objj, Out, OutObject);
						ObjectProp->SetObjectPropertyValue_InContainer(OutObject, Out);
					}
				}
			}
			else if(UStructProperty* StructProp = Cast<UStructProperty>(Property))
			{
				void* Value = StructProp->ContainerPtrToValuePtr<uint8>(OutObject);
				if (!JsonValue->AsObject())
					continue;

				JsonAttributesToUObject(JsonValue->AsObject()->Values, StructProp->Struct, Value);
			}
			else
			{
				void* Value = Property->ContainerPtrToValuePtr<uint8>(OutObject);
				ConvertScalarJsonValueToUProperty(JsonValue, Property, Value);
			}
		}
	}
}
IFTypes::IFTypes()
{
}

IFTypes::~IFTypes()
{
}
