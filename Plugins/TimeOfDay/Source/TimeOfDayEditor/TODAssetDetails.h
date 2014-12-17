#pragma once
#include "PropertyEditing.h"
#include "PropertyCustomizationHelpers.h"
#include "AssetTypeActions_Base.h"

DECLARE_DELEGATE_RetVal(FName, FOnGetCurrentProperty);

class FTODAssetDetails : public IDetailCustomization
{
public:
	FTODAssetDetails(FOnGetCurrentProperty OnGetCurrentPropertyIn)
		: OnGetCurrentProperty(OnGetCurrentPropertyIn)
	{}

	/** Makes a new instance of this detail layout class for a specific detail view requesting it */
	static TSharedRef<IDetailCustomization> MakeInstance(FOnGetCurrentProperty OnGetCurrentPropertyIn);

	/** IDetailCustomization interface */
	virtual void CustomizeDetails(IDetailLayoutBuilder& DetailLayout) override;

	TArray<TWeakObjectPtr<UObject>> CustomizedObjects;

	FOnGetCurrentProperty OnGetCurrentProperty;
};