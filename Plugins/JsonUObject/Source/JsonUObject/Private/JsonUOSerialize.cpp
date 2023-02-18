// Fill out your copyright notice in the Description page of Project Settings.

#include "JsonUOSerialize.h"
#include "JsonObjectConverter.h"

bool FJsonUOSerialize::UObjectToJsonObject(const UStruct* StructDefinition, const void* Struct, TSharedRef<FJsonObject> OutJsonObject)
{
	OutJsonObject->Values.Add(FString("objectClass"), MakeShareable(new FJsonValueString(StructDefinition->GetPathName())));
	return UStructToJsonAttributes(StructDefinition, Struct, OutJsonObject->Values);
}

TSharedPtr<FJsonValue> FJsonUOSerialize::ConvertScalarUPropertyToJsonValue(UProperty* Property, const void* Value)
{
	// See if there's a custom export callback first, so it can override default behavior


	if (UEnumProperty* EnumProperty = Cast<UEnumProperty>(Property))
	{
		// export enums as strings
		UEnum* EnumDef = EnumProperty->GetEnum();
		FString StringValue = EnumDef->GetNameStringByValue(EnumProperty->GetUnderlyingProperty()->GetSignedIntPropertyValue(Value));
		return MakeShareable(new FJsonValueString(StringValue));
	}
	else if (UNumericProperty *NumericProperty = Cast<UNumericProperty>(Property))
	{
		// see if it's an enum
		UEnum* EnumDef = NumericProperty->GetIntPropertyEnum();
		if (EnumDef != NULL)
		{
			// export enums as strings
			FString StringValue = EnumDef->GetNameStringByValue(NumericProperty->GetSignedIntPropertyValue(Value));
			return MakeShareable(new FJsonValueString(StringValue));
		}

		// We want to export numbers as numbers
		if (NumericProperty->IsFloatingPoint())
		{
			return MakeShareable(new FJsonValueNumber(NumericProperty->GetFloatingPointPropertyValue(Value)));
		}
		else if (NumericProperty->IsInteger())
		{
			return MakeShareable(new FJsonValueNumber(NumericProperty->GetSignedIntPropertyValue(Value)));
		}

		// fall through to default
	}
	else if (UBoolProperty *BoolProperty = Cast<UBoolProperty>(Property))
	{
		// Export bools as bools
		return MakeShareable(new FJsonValueBoolean(BoolProperty->GetPropertyValue(Value)));
	}
	else if (UStrProperty *StringProperty = Cast<UStrProperty>(Property))
	{
		return MakeShareable(new FJsonValueString(StringProperty->GetPropertyValue(Value)));
	}
	else if (UTextProperty *TextProperty = Cast<UTextProperty>(Property))
	{
		return MakeShareable(new FJsonValueString(TextProperty->GetPropertyValue(Value).ToString()));
	}
	else if (UArrayProperty *ArrayProperty = Cast<UArrayProperty>(Property))
	{
		TArray< TSharedPtr<FJsonValue> > Out;
		FScriptArrayHelper Helper(ArrayProperty, Value);
		for (int32 i = 0, n = Helper.Num(); i<n; ++i)
		{
			TSharedPtr<FJsonValue> Elem = UPropertyToJsonValue(ArrayProperty->Inner, Helper.GetRawPtr(i));
			if (Elem.IsValid())
			{
				// add to the array
				Out.Push(Elem);
			}
		}
		return MakeShareable(new FJsonValueArray(Out));
	}
	else if (USetProperty* SetProperty = Cast<USetProperty>(Property))
	{
		TArray< TSharedPtr<FJsonValue> > Out;
		FScriptSetHelper Helper(SetProperty, Value);
		for (int32 i = 0, n = Helper.Num(); n; ++i)
		{
			if (Helper.IsValidIndex(i))
			{
				TSharedPtr<FJsonValue> Elem = UPropertyToJsonValue(SetProperty->ElementProp, Helper.GetElementPtr(i));
				if (Elem.IsValid())
				{
					// add to the array
					Out.Push(Elem);
				}

				--n;
			}
		}
		return MakeShareable(new FJsonValueArray(Out));
	}
	else if (UMapProperty* MapProperty = Cast<UMapProperty>(Property))
	{
		TSharedRef<FJsonObject> Out = MakeShareable(new FJsonObject());

		FScriptMapHelper Helper(MapProperty, Value);
		for (int32 i = 0, n = Helper.Num(); n; ++i)
		{
			if (Helper.IsValidIndex(i))
			{
				TSharedPtr<FJsonValue> KeyElement = UPropertyToJsonValue(MapProperty->KeyProp, Helper.GetKeyPtr(i));
				TSharedPtr<FJsonValue> ValueElement = UPropertyToJsonValue(MapProperty->ValueProp, Helper.GetValuePtr(i));
				if (KeyElement.IsValid() && ValueElement.IsValid())
				{
					FString KeyString = KeyElement->AsString();
					if (KeyString.IsEmpty())
					{
						MapProperty->KeyProp->ExportTextItem(KeyString, Helper.GetKeyPtr(i), nullptr, nullptr, 0);
						if (KeyString.IsEmpty())
						{
							UE_LOG(LogJson, Error, TEXT("Unable to convert key to string for property %s."), *MapProperty->GetName())
								KeyString = FString::Printf(TEXT("Unparsed Key %d"), i);
						}
					}

					Out->SetField(KeyString, ValueElement);
				}

				--n;
			}
		}

		return MakeShareable(new FJsonValueObject(Out));
	}
	else if (UObjectProperty* ObjectProperty = Cast<UObjectProperty>(Property))
	{
		TSharedRef<FJsonObject> Out = MakeShareable(new FJsonObject());

		UObject* Object = ObjectProperty->GetObjectPropertyValue(Value);
		if (Object)
		{
			UClass* cls = Object->GetClass();

			if (UObjectToJsonObject(cls, Object, Out))
			{
				return MakeShareable(new FJsonValueObject(Out));
			}
		}
	}
	else if (UStructProperty *StructProperty = Cast<UStructProperty>(Property))
	{
		UScriptStruct::ICppStructOps* TheCppStructOps = StructProperty->Struct->GetCppStructOps();
		// Intentionally exclude the JSON Object wrapper, which specifically needs to export JSON in an object representation instead of a string
		if (StructProperty->Struct != FJsonObjectWrapper::StaticStruct() && TheCppStructOps && TheCppStructOps->HasExportTextItem())
		{
			FString OutValueStr;
			TheCppStructOps->ExportTextItem(OutValueStr, Value, nullptr, nullptr, PPF_None, nullptr);
			return MakeShareable(new FJsonValueString(OutValueStr));
		}

		TSharedRef<FJsonObject> Out = MakeShareable(new FJsonObject());
		if (UObjectToJsonObject(StructProperty->Struct, Value, Out))
		{
			return MakeShareable(new FJsonValueObject(Out));
		}
		// fall through to default
	}
	else
	{
		// Default to export as string for everything else
		FString StringValue;
		Property->ExportTextItem(StringValue, Value, NULL, NULL, PPF_None);
		return MakeShareable(new FJsonValueString(StringValue));
	}

	// invalid
	return TSharedPtr<FJsonValue>();
}

TSharedPtr<FJsonValue> FJsonUOSerialize::UPropertyToJsonValue(UProperty* Property, const void* Value)
{
	if (Property->ArrayDim == 1)
	{
		return ConvertScalarUPropertyToJsonValue(Property, Value);
	}

	TArray< TSharedPtr<FJsonValue> > Array;
	for (int Index = 0; Index != Property->ArrayDim; ++Index)
	{
		Array.Add(ConvertScalarUPropertyToJsonValue(Property, (char*)Value + Index * Property->ElementSize));
	}
	return MakeShareable(new FJsonValueArray(Array));
}

bool FJsonUOSerialize::UStructToJsonAttributes(const UStruct* StructDefinition, const void* Struct, TMap< FString, TSharedPtr<FJsonValue> >& OutJsonAttributes)
{
	int64 SerFlag = 0;
	SerFlag |= CPF_SaveGame;

	for (TFieldIterator<UProperty> It(StructDefinition); It; ++It)
	{
		UProperty* Property = *It;

		if (Property->HasAnyPropertyFlags(SerFlag))
		{
			FString VariableName = Property->GetName();// StandardizeCase();
			const void* Value = Property->ContainerPtrToValuePtr<uint8>(Struct);

			// convert the property to a FJsonValue
			TSharedPtr<FJsonValue> JsonValue = UPropertyToJsonValue(Property, Value);
			if (JsonValue.IsValid())
			{
				OutJsonAttributes.Add(VariableName, JsonValue);

				//return false;
			}
			else
			{
				UClass* PropClass = Property->GetClass();
				UE_LOG(LogJson, Error, TEXT("UStructToJsonObject - Unhandled property type '%s': %s"), *PropClass->GetName(), *Property->GetPathName());
			}
		}
	}

	return true;
}