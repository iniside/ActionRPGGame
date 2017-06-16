
#pragma once

#include "SlateBasics.h"
#include "GAGlobalTypesEditor.h"

DECLARE_DELEGATE_OneParam(FGAOnAttributeSelected, FString);

class FAFAttributeNode
{
	FString NodeName;
	
	FString Attribute;

	TWeakPtr<FAFAttributeNode> ParentNode;

	TArray<TSharedPtr<FAFAttributeNode> > ChildNodes;
};

class FAFAttributeTree
{

};

class SGAAttributeWidget : public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SGAAttributeWidget)
	{}
		SLATE_EVENT(FGAOnAttributeSelected, OnAttributeSelectedIn)
	SLATE_END_ARGS();

	void Construct(const FArguments& InArgs);
	~SGAAttributeWidget();
private:
	TArray<TSharedPtr<FString>> AttributesList;

	TArray<TSharedPtr<FGAAttributeNode> > AttributesNodes;

	TSharedPtr<class STreeView<TSharedPtr<FGAAttributeNode> > > AttributeTreeWidget;

	FGAOnAttributeSelected OnAttributeSelected;

	void OnItemSelected(TSharedPtr<FGAAttributeNode> SelectedItem, ESelectInfo::Type SelectInfo);
	/**
	* Generate a row widget for the specified item node and table
	*
	* @param InItem		Tag node to generate a row widget for
	* @param OwnerTable	Table that owns the row
	*
	* @return Generated row widget for the item node
	*/
	TSharedRef<ITableRow> OnGenerateRow(TSharedPtr<FGAAttributeNode> InItem, const TSharedRef<STableViewBase>& OwnerTable);

	/**
	* Get children nodes of the specified node
	*
	* @param InItem		Node to get children of
	* @param OutChildren	[OUT] Array of children nodes, if any
	*/
	void OnGetChildren(TSharedPtr<FGAAttributeNode> InItem, TArray< TSharedPtr<FGAAttributeNode> >& OutChildren);


	void OnExpansionChanged(TSharedPtr<FGAAttributeNode> InItem, bool bIsExpanded);
};