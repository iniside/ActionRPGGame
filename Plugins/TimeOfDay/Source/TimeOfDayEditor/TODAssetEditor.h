#pragma once
#include "TimeOfDayEditor.h"
//#include "UnrealEd.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "Toolkits/AssetEditorManager.h"

struct FTODPropertyDisplay
{
	//name of property
	FName PropertyName;

	//value of selected propety in current time
	float PropertyValueInTime;
};

class FTODAssetEditor : public FAssetEditorToolkit, public FCurveOwnerInterface
{
public:

	FTODAssetEditor();
	//IToolkit Interface
	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	//End IToolkit Interface


	// FAssetEditorToolkit
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual FText GetToolkitName() const override;
	virtual FLinearColor GetWorldCentricTabColorScale() const override;
	virtual FString GetWorldCentricTabPrefix() const override;
	virtual FString GetDocumentationLink() const override;
	// End of FAssetEditorToolkit

	/** FCurveOwnerInterface Begin */
	/** Returns set of curves to edit. Must not release the curves while being edited. */
	virtual TArray<FRichCurveEditInfoConst> GetCurves() const override;

	/** Returns set of curves to query. Must not release the curves while being edited. */
	virtual TArray<FRichCurveEditInfo> GetCurves() override;

	/** Called to return the Curve UObject  **/
	virtual UObject* GetOwner() override;

	/** Called to modify the owner of the curve */
	virtual void ModifyOwner() override;

	/** Called to make curve owner transactional */
	virtual void MakeTransactional()override;

	/** Called when the curve has been changed */
	virtual void OnCurveChanged() override;

	/** Whether the curve represents a linear color */
	virtual bool IsLinearColorCurve() const override;

	/** Evaluate this color curve at the specified time */
	virtual FLinearColor GetLinearColorValue(float InTime) const override;

	/** @return True if the curve has any alpha keys */
	virtual bool HasAnyAlphaKeys() const override;// { return false; }
	/* FCurveOwnerInterface Begin **/

	~FTODAssetEditor();
	void InitiItemEditor(const EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UTODAsset* ItemIn);

	TSharedRef<SDockTab> SpawnTab_Item(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_Details(const FSpawnTabArgs& Args);
	TSharedRef<SDockTab> SpawnTab_BaseProperties(const FSpawnTabArgs& Args);
protected:

	TSharedPtr<class SCurveEditor> CurveEditorWidget;

	UTODAsset* EditedItemAsset;

	TSharedPtr< class IDetailsView > ItemDetailsView;
	TSharedPtr< class IDetailsView > PropertyDetailsView;

	TArray<UProperty*> PropList;

	//Only one of these properties can have value at time.
	//Null them everytime value of any of them change.
	FRuntimeFloatCurve* SelectedFloatCurve;
	FRuntimeCurveLinearColor* SelectedColorCurve;
	
	FName SelectedCurveName;
	FText GetCurrentCurveName() const;
	FName GetCurrentProperty();

	void UpdateFloatCurve(float ValueIn);
	void UpdateColorCurve(FLinearColor ValueIn);

	void SetCurveToEdit(UProperty* CurveProp);
private:
	/**	The tab id for the curve table tab */
	static const FName TODEditorTabId;
	static const FName TODEditorTabDetails;
	static const FName TODEditorTabBaseProperties;
};