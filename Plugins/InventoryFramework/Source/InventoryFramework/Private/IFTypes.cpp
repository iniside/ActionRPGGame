// Fill out your copyright notice in the Description page of Project Settings.

#include "IFTypes.h"
#include "JsonObjectConverter.h"

TSharedPtr<FJsonValue> UPropertyToJsonValue(UProperty* Property, const void* Value)
{
	if (Property->ArrayDim == 1)
	{
		return FIFJsonSerializer::ConvertScalarUPropertyToJsonValue(Property, Value);
	}

	TArray< TSharedPtr<FJsonValue> > Array;
	for (int Index = 0; Index != Property->ArrayDim; ++Index)
	{
		Array.Add(FIFJsonSerializer::ConvertScalarUPropertyToJsonValue(Property, (char*)Value + Index * Property->ElementSize));
	}
	return MakeShareable(new FJsonValueArray(Array));
}

TSharedPtr<FJsonValue> FIFJsonSerializer::ConvertScalarUPropertyToJsonValue(UProperty* Property, const void* Value)
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

bool FIFJsonSerializer::UStructToJsonAttributes(const UStruct* StructDefinition, const void* Struct, TMap< FString, TSharedPtr<FJsonValue> >& OutJsonAttributes)
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
		// set the value on the output object
		
	}

	return true;
}

bool FIFJsonSerializer::UObjectToJsonObject(const UStruct* StructDefinition, const void* Struct, TSharedRef<FJsonObject> OutJsonObject)
{
	OutJsonObject->Values.Add(FString("objectClass"), MakeShareable(new FJsonValueString(StructDefinition->GetPathName())));
	return UStructToJsonAttributes(StructDefinition, Struct, OutJsonObject->Values);
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
			void* val = Property->ContainerPtrToValuePtr<uint8>(OutObject);
			if (UStructProperty* StructProp = Cast<UStructProperty>(Property))
			{
				static const FName NAME_DateTime(TEXT("DateTime"));
				static const FName NAME_Color(TEXT("Color"));
				static const FName NAME_LinearColor(TEXT("LinearColor"));
				if (JsonValue->Type == EJson::Object)
				{
					TSharedPtr<FJsonObject> Obj = JsonValue->AsObject();
					check(Obj.IsValid()); // should not fail if Type == EJson::Object
					if (!FJsonObjectConverter::JsonObjectToUStruct(Obj.ToSharedRef(), StructProp->Struct, val, 0, 0))
					{
						UE_LOG(LogJson, Error, TEXT("JsonValueToUProperty - FJsonObjectConverter::JsonObjectToUStruct failed for property %s"), *Property->GetNameCPP());
						continue;
					}
				}
				else if (JsonValue->Type == EJson::String && StructProp->Struct->GetFName() == NAME_LinearColor)
				{
					FLinearColor& ColorOut = *(FLinearColor*)val;
					FString ColorString = JsonValue->AsString();

					FColor IntermediateColor;
					IntermediateColor = FColor::FromHex(ColorString);

					ColorOut = IntermediateColor;
				}
				else if (JsonValue->Type == EJson::String && StructProp->Struct->GetFName() == NAME_Color)
				{
					FColor& ColorOut = *(FColor*)val;
					FString ColorString = JsonValue->AsString();

					ColorOut = FColor::FromHex(ColorString);
				}
				else if (JsonValue->Type == EJson::String && StructProp->Struct->GetFName() == NAME_DateTime)
				{
					FString DateString = JsonValue->AsString();
					FDateTime& DateTimeOut = *(FDateTime*)val;
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
						continue;
					}
				}
			}
			else if (USoftClassProperty* SofftClassProp = Cast<USoftClassProperty>(Property))
			{
				
				FString dupa;
				JsonValue->TryGetString(dupa);
				FSoftClassPath classPath(dupa);

				UClass* dupaCls = Cast<UClass>(path.TryLoad());
				SofftClassProp->SetMetaClass(dupaCls);
			}
			else if (UObjectProperty* ObjectProp = Cast<UObjectProperty>(Property))
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
						FSoftClassPath path(objClassStr2);
						UClass* itemCls = Cast<UClass>(path.TryLoad());
						UObject* Out = nullptr;
						JsonObjectToUObject(Objj, Out, OutObject);
						ObjectProp->SetObjectPropertyValue_InContainer(OutObject, Out);
					}
				}
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
