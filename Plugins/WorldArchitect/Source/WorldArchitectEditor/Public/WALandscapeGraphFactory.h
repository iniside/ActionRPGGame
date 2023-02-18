#pragma once

#include "WALandscapeGraphFactory.generated.h"

UCLASS()
class WORLDARCHITECTEDITOR_API UWALandscapeGraphFactory : public UFactory
{
	GENERATED_BODY()

public:
	UWALandscapeGraphFactory();
	virtual ~UWALandscapeGraphFactory();

	virtual UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
