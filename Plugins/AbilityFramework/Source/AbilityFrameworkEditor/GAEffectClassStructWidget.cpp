// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#include "AbilityFrameworkEditor.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Editor/PropertyEditor/Public/PropertyEditing.h"
#include "Editor/PropertyEditor/Private/IDetailsViewPrivate.h"
#include "Editor/PropertyEditor/Private/SDetailsViewBase.h"
#include "Editor/PropertyEditor/Private/SDetailsView.h"

#include "STextCombobox.h"
#include "STreeView.h"
#include "SButton.h"
#include "STextBlock.h"

#include "ClassViewerModule.h"
#include "ClassViewerFilter.h"

#include "EditorClassUtils.h"
#include "IAssetTools.h"
#include "AssetRegistryModule.h"
#include "Dialogs/DlgPickAssetPath.h"
#include "AssetToolsModule.h"
#include "KismetCompilerModule.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/CompilerResultsLog.h"
#include "EffectEditor/GAEffectEditor.h"
#include "SMyBlueprint.h"
#include "SKismetInspector.h"

#include "SGAAttributeWidget.h"
#include "Effects/GAGameEffect.h"
#include "Effects/GAEffectBlueprint.h"
#include "EffectEditor/GAEffectBlueprintFactory.h"
#include "GAEffectClassStructWidget.h"
#include "GAEffectDetails.h"
#include "AFAbilityActionSpecDetails.h"
#include "AFAbilityPeriodSpecDetails.h"
#include "AFAbilityCooldownSpecDetails.h"
#include "Abilities/AFAbilityActivationSpec.h"
#include "Abilities/AFAbilityPeriodSpec.h"
#include "Abilities/AFAbilityCooldownSpec.h"

class SEffectCreateDialog : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SEffectCreateDialog) {}
		SLATE_ARGUMENT(TSet<const UClass*>, MetaClass);
	SLATE_END_ARGS()

		/** Constructs this widget with InArgs */
		void Construct(const FArguments& InArgs)
	{
		bOkClicked = false;
		ParentClass = UGAGameEffectSpec::StaticClass();
		MetaClass = InArgs._MetaClass;
		ChildSlot
			[
				SNew(SBorder)
				.Visibility(EVisibility::Visible)
			.BorderImage(FEditorStyle::GetBrush("Menu.Background"))
			[
				SNew(SBox)
				.Visibility(EVisibility::Visible)
			.WidthOverride(500.0f)
			[
				SNew(SVerticalBox)
				+ SVerticalBox::Slot()
			.FillHeight(1)
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.Content()
			[
				SAssignNew(ParentClassContainer, SVerticalBox)
			]
			]

		// Ok/Cancel buttons
		+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.VAlign(VAlign_Bottom)
			.Padding(8)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
			.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
			.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
			+ SUniformGridPanel::Slot(0, 0)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
			.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			.OnClicked(this, &SEffectCreateDialog::OkClicked)
			.Text(FText::FromString("OK"))
			]
		+ SUniformGridPanel::Slot(1, 0)
			[
				SNew(SButton)
				.HAlign(HAlign_Center)
			.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
			.OnClicked(this, &SEffectCreateDialog::CancelClicked)
			.Text(FText::FromString("Cancel"))
			]
			]
			]
			]
			];

		MakeParentClassPicker();
	}

	/** Sets properties for the supplied GameplayAbilitiesBlueprintFactory */
	bool ConfigureProperties(FGAEffectClassStructWidget* InGameplayAbilitiesBlueprintFactory)
	{
		GameplayAbilitiesBlueprintFactory = InGameplayAbilitiesBlueprintFactory;

		TSharedRef<SWindow> Window = SNew(SWindow)
			.Title(FText::FromString("Create Game Effect Blueprint"))
			.ClientSize(FVector2D(400, 700))
			.SupportsMinimize(false).SupportsMaximize(false)
			[
				AsShared()
			];

		PickerWindow = Window;

		GEditor->EditorAddModalWindow(Window);
		GameplayAbilitiesBlueprintFactory = nullptr;

		return bOkClicked;
	}

private:
	class FGameplayAbilityBlueprintParentFilter : public IClassViewerFilter
	{
	public:
		/** All children of these classes will be included unless filtered out by another setting. */
		TSet< const UClass* > AllowedChildrenOfClasses;

		FGameplayAbilityBlueprintParentFilter() {}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};

	/** Creates the combo menu for the parent class */
	void MakeParentClassPicker()
	{
		// Load the classviewer module to display a class picker
		FClassViewerModule& ClassViewerModule = FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer");

		// Fill in options
		FClassViewerInitializationOptions Options;
		Options.Mode = EClassViewerMode::ClassPicker;
		Options.bShowDisplayNames = true;
		// Only allow parenting to base blueprints.
		Options.bIsBlueprintBaseOnly = true;

		TSharedPtr<FGameplayAbilityBlueprintParentFilter> Filter = MakeShareable(new FGameplayAbilityBlueprintParentFilter);

		// All child child classes of UGameplayAbility are valid.
		Filter->AllowedChildrenOfClasses = MetaClass;
		Options.ClassFilter = Filter;

		ParentClassContainer->ClearChildren();
		ParentClassContainer->AddSlot()
			.AutoHeight()
			[
				SNew(STextBlock)
				.Text(FText::FromString("Parent Class:"))
			.ShadowOffset(FVector2D(1.0f, 1.0f))
			];

		ParentClassContainer->AddSlot()
			[
				ClassViewerModule.CreateClassViewer(Options, FOnClassPicked::CreateSP(this, &SEffectCreateDialog::OnClassPicked))
			];
	}

	/** Handler for when a parent class is selected */
	void OnClassPicked(UClass* ChosenClass)
	{
		ParentClass = ChosenClass;
	}

	/** Handler for when ok is clicked */
	FReply OkClicked()
	{
		if (GameplayAbilitiesBlueprintFactory)
		{
			GameplayAbilitiesBlueprintFactory->ParentClass = ParentClass.Get();
		}

		CloseDialog(true);

		return FReply::Handled();
	}

	void CloseDialog(bool bWasPicked = false)
	{
		bOkClicked = bWasPicked;
		if (PickerWindow.IsValid())
		{
			PickerWindow.Pin()->RequestDestroyWindow();
		}
	}

	/** Handler for when cancel is clicked */
	FReply CancelClicked()
	{
		CloseDialog();
		return FReply::Handled();
	}

	FReply OnKeyDown(const FGeometry& MyGeometry, const FKeyEvent& InKeyEvent)
	{
		if (InKeyEvent.GetKey() == EKeys::Escape)
		{
			CloseDialog();
			return FReply::Handled();
		}
		return SWidget::OnKeyDown(MyGeometry, InKeyEvent);
	}

private:
	/** The factory for which we are setting up properties */
	FGAEffectClassStructWidget* GameplayAbilitiesBlueprintFactory;

	/** A pointer to the window that is asking the user to select a parent class */
	TWeakPtr<SWindow> PickerWindow;

	/** The container for the Parent Class picker */
	TSharedPtr<SVerticalBox> ParentClassContainer;

	/** The selected class */
	TWeakObjectPtr<UClass> ParentClass;

	TSet<const UClass*> MetaClass;

	/** True if Ok was clicked */
	bool bOkClicked;
};
class FPropertyEditorClassFilter : public IClassViewerFilter
{
public:
	/** The meta class for the property that classes must be a child-of. */
	const UClass* ClassPropertyMetaClass;

	/** The interface that must be implemented. */
	const UClass* InterfaceThatMustBeImplemented;

	/** Whether or not abstract classes are allowed. */
	bool bAllowAbstract;

	bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated) &&
			(bAllowAbstract || !InClass->HasAnyClassFlags(CLASS_Abstract));

		if (bMatchesFlags && InClass->IsChildOf(ClassPropertyMetaClass)
			&& (!InterfaceThatMustBeImplemented || InClass->ImplementsInterface(InterfaceThatMustBeImplemented)))
		{
			return true;
		}

		return false;
	}

	virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
	{
		bool bMatchesFlags = !InClass->HasAnyClassFlags(CLASS_Hidden | CLASS_HideDropDown | CLASS_Deprecated) &&
			(bAllowAbstract || !InClass->HasAnyClassFlags(CLASS_Abstract));

		if (bMatchesFlags && InClass->IsChildOf(ClassPropertyMetaClass)
			&& (!InterfaceThatMustBeImplemented || InClass->ImplementsInterface(InterfaceThatMustBeImplemented)))
		{
			return true;
		}

		return false;
	}
};

FGAEffectClassStructWidget::~FGAEffectClassStructWidget()
{

}

TSharedRef<SWidget> FGAEffectClassStructWidget::CreateEffectClassWidget(UObject* OwnerObject)
{
	OuterObject = OwnerObject;
	//TAttribute<FText> EffectName;// ();
	//EffectName.Create(TAttribute<FText>::FGetter::CreateRaw(this, &FGAEffectClassStructWidget::GetClassName));
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SAssignNew(ComboButton, SComboButton)
			.OnGetMenuContent(this, &FGAEffectClassStructWidget::GenerateClassPicker)
			.ContentPadding(FMargin(2.0f, 2.0f))
			.ToolTipText(this, &FGAEffectClassStructWidget::GetDisplayValueAsString)
			.ButtonContent()
			[
				SNew(STextBlock)
				.Text(this, &FGAEffectClassStructWidget::GetDisplayValueAsString)
			//.Font(InArgs._Font)
			]
		]
		+SHorizontalBox::Slot()
			.AutoWidth()
		[
			MakeNewBlueprintButton()
		]
		+SHorizontalBox::Slot()
			.AutoWidth()
		[
			SNew(SButton)
			.OnClicked(this, &FGAEffectClassStructWidget::OnEditButtonClicked)
			[
				SNew(STextBlock)
				.Text(FText::FromString("Edit"))
			]
		];
}

FText FGAEffectClassStructWidget::GetClassName() const
{
	if (StructPropertyHandle.IsValid())
	{
		return StructPropertyHandle->GetPropertyDisplayName();
	}
	return FText::FromString("No Effect Selected");
}
static FString GetClassDisplayName(const UObject* Object)
{
	const UClass* Class = Cast<UClass>(Object);
	if (Class != NULL)
	{
		UBlueprint* BP = UBlueprint::GetBlueprintFromClass(Class);
		if (BP != NULL)
		{
			return BP->GetName();
		}
	}
	return (Object) ? Object->GetName() : "None";
}
FText FGAEffectClassStructWidget::GetDisplayValueAsString() const
{
	static bool bIsReentrant = false;

	// Guard against re-entrancy which can happen if the delegate executed below (SelectedClass.Get()) forces a slow task dialog to open, thus causing this to lose context and regain focus later starting the loop over again
	if (!bIsReentrant)
	{
		TGuardValue<bool>(bIsReentrant, true);
		if (EffectPropertyHandle.IsValid())
		{
			UObject* ObjectValue = NULL;
			FPropertyAccess::Result Result = EffectPropertyHandle->GetValue(ObjectValue);

			if (Result == FPropertyAccess::Success && ObjectValue != NULL)
			{
				return FText::FromString(GetClassDisplayName(ObjectValue));
			}

			return FText::FromString("None");
		}

		return FText::FromString("None");
	}
	else
	{
		return FText::FromString("None");
	}

}
UClass* FGAEffectClassStructWidget::GetClassFromString(const FString& ClassName)
{
	if (ClassName.IsEmpty() || ClassName == "None")
	{
		return nullptr;
	}

	UClass* Class = FindObject<UClass>(ANY_PACKAGE, *ClassName);
	if (!Class)
	{
		Class = LoadObject<UClass>(nullptr, *ClassName);
	}
	return Class;
}
TSharedRef<SWidget> FGAEffectClassStructWidget::GenerateClassPicker()
{
	FClassViewerInitializationOptions Options;
	Options.bShowUnloadedBlueprints = true;
	Options.bShowNoneOption = true;

	if (EffectPropertyHandle.IsValid())
	{
		Options.PropertyHandle = EffectPropertyHandle;
	}

	
	class FGameplayAbilityBlueprintParentFilter : public IClassViewerFilter
	{
	public:
		/** All children of these classes will be included unless filtered out by another setting. */
		TSet< const UClass* > AllowedChildrenOfClasses;
		bool bAllowAbstract;
		FGameplayAbilityBlueprintParentFilter() {}

		virtual bool IsClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const UClass* InClass, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			if (!bAllowAbstract)
			{
				return !InClass->HasAnyClassFlags(CLASS_Abstract) && InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
			}
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InClass) != EFilterReturn::Failed;
		}

		virtual bool IsUnloadedClassAllowed(const FClassViewerInitializationOptions& InInitOptions, const TSharedRef< const IUnloadedBlueprintData > InUnloadedClassData, TSharedRef< FClassViewerFilterFuncs > InFilterFuncs) override
		{
			// If it appears on the allowed child-of classes list (or there is nothing on that list)
			return InFilterFuncs->IfInChildOfClassesSet(AllowedChildrenOfClasses, InUnloadedClassData) != EFilterReturn::Failed;
		}
	};
	TSharedPtr<FGameplayAbilityBlueprintParentFilter> ClassFilter = MakeShareable(new FGameplayAbilityBlueprintParentFilter);
	
	TSet<const UClass*> MetaClass;
	TSharedPtr<IPropertyHandle> EffectPropertyHandleLocal = StructPropertyHandle->GetParentHandle();
	if (EffectPropertyHandleLocal.IsValid())
	{
		FString ClassName = EffectPropertyHandleLocal->GetMetaData("AllowedClass");
		TArray<FString> ClassNames;
		//ClassName.Pars
		ClassName.ParseIntoArray(ClassNames, TEXT(","));
		if (ClassNames.Num() > 0)
		{
			for (const FString& name : ClassNames)
			{
				UClass* temp = GetClassFromString(name);
				if (temp)
				{
					MetaClass.Add(temp);
				}
			}
		}
		else
		{
			MetaClass.Add(UGAGameEffectSpec::StaticClass());
		}
	}
	ClassFilter->AllowedChildrenOfClasses = MetaClass;
	Options.ClassFilter = ClassFilter;
	//ClassFilter->ClassPropertyMetaClass = MetaClass;
	//ClassFilter->InterfaceThatMustBeImplemented = nullptr;// UInterface::StaticClass();
	ClassFilter->bAllowAbstract = false;
	Options.bIsBlueprintBaseOnly = true;
	Options.bIsPlaceableOnly = false;
	Options.DisplayMode = EClassViewerDisplayMode::TreeView;// : EClassViewerDisplayMode::ListView;
	Options.bAllowViewOptions = true;
	Options.bShowDisplayNames = true;

	FOnClassPicked OnPicked(FOnClassPicked::CreateRaw(this, &FGAEffectClassStructWidget::OnClassPicked));

	return SNew(SBox)
		.WidthOverride(280)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
		.AutoHeight()
		.MaxHeight(500)
		[
			FModuleManager::LoadModuleChecked<FClassViewerModule>("ClassViewer").CreateClassViewer(Options, OnPicked)
		]
		];
}

void FGAEffectClassStructWidget::OnClassPicked(UClass* InClass)
{
	if (!InClass)
	{
		SendToObjects(TEXT("None"));
	}
	else
	{
		SendToObjects(InClass->GetPathName());
	}

	ComboButton->SetIsOpen(false);
}

void FGAEffectClassStructWidget::SendToObjects(const FString& NewValue)
{
	if (EffectPropertyHandle.IsValid())
	{
		EffectPropertyHandle->SetValueFromFormattedString(NewValue);
	}
	else
	{
		UClass* NewClass = FindObject<UClass>(ANY_PACKAGE, *NewValue);
		if (!NewClass)
		{
			NewClass = LoadObject<UClass>(nullptr, *NewValue);
		}
		//OnSetClass.Execute(NewClass);
	}
}

TSharedRef<SWidget> FGAEffectClassStructWidget::MakeNewBlueprintButton()
{
	return
		SNew(SButton)
		.Text(FText::FromString("New Blueprint"))
		//.ToolTipText(OptionalToolTipText.Get().IsEmpty() ? LOCTEXT("NewBlueprintButtonToolTipText", "Create New Blueprint") : OptionalToolTipText)
		.OnClicked(this, &FGAEffectClassStructWidget::MakeNewBlueprint)
		.IsEnabled(true)
		.IsFocusable(false)
		.ButtonColorAndOpacity(FSlateColor::UseForeground())
		[
			SNew(SImage)
			.Image(FEditorStyle::GetBrush("PropertyWindow.Button_CreateNewBlueprint"))
		];
}

FReply FGAEffectClassStructWidget::MakeNewBlueprint()
{
	TSet<const UClass*> MetaClass;
	TSharedPtr<IPropertyHandle> EffectPropertyHandleLocal = StructPropertyHandle->GetParentHandle();
	FString ClassName = EffectPropertyHandleLocal->GetMetaData("AllowedClass");
	TArray<FString> ClassNames;
	//ClassName.Pars
	ClassName.ParseIntoArray(ClassNames, TEXT(","));
	if (ClassNames.Num() > 0)
	{
		for (const FString& name : ClassNames)
		{
			UClass* temp = GetClassFromString(name);
			if (temp)
			{
				MetaClass.Add(temp);
			}
		}
	}
	else
	{
		MetaClass.Add(UAFEffectSpecBase::StaticClass());
	}
	TSharedRef<SEffectCreateDialog> Dialog = SNew(SEffectCreateDialog).MetaClass(MetaClass);
	Dialog->ConfigureProperties(this);
	if (ParentClass)
	{
		check(FKismetEditorUtilities::CanCreateBlueprintOfClass(UGAGameEffectSpec::StaticClass()));

		// Pre-generate a unique asset name to fill out the path picker dialog with.
		FString OuterName;
		FString OuterName2;

		if (StructPropertyHandle.IsValid())
		{
			UProperty* prop = StructPropertyHandle->GetProperty();
			OuterName = prop->GetOuter()->GetName();
			OuterName = prop->GetPathName();
			OuterName = StructPropertyHandle->GeneratePathToProperty();
		}
		
		
		UPackage* OuterMost = OuterObject->GetOutermost();
		FString AssetName = OuterObject->GetName();
		FString Path = OuterMost->GetFullName();
		FJsonSerializableArray OutArray;
		Path = Path.RightChop(7);
		Path.ParseIntoArrayWS(OutArray, TEXT("/"));
		FString OutAsset = OutArray[OutArray.Num() - 1];
		OutArray.RemoveAt(OutArray.Num() - 1);
	//	OutArray.RemoveAt(0);
		FString OutPath = "/";
		for (const FString& str : OutArray)
		{
			OutPath += str;
			OutPath += "/";
		}
		OutPath += FString::Printf(TEXT("E_%s_"), *OutAsset);

		FString NewNameSuggestion = "";// FString::Printf(TEXT("E_%s_"), *OutAsset);
		UClass* BlueprintClass = UGAGameEffectSpec::StaticClass();
		UClass* BlueprintGeneratedClass = UGAEffectBlueprint::StaticClass();

		IKismetCompilerInterface& KismetCompilerModule = FModuleManager::LoadModuleChecked<IKismetCompilerInterface>("KismetCompiler");
		KismetCompilerModule.GetBlueprintTypesForClass(UGAGameEffectSpec::StaticClass(), BlueprintClass, BlueprintGeneratedClass);

		FString PackageName = OutPath + NewNameSuggestion;
		FString OutPackageName;
		FString Name;
		FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		AssetToolsModule.Get().CreateUniqueAssetName(PackageName, NewNameSuggestion, OutPackageName, Name);

		

		TSharedPtr<SDlgPickAssetPath> PickAssetPathWidget =
			SNew(SDlgPickAssetPath)
			.Title(FText::FromString("Create New Effect"))
			.DefaultAssetPath(FText::FromString(OutPath));
		UBlueprint* Blueprint = nullptr;
		if (EAppReturnType::Ok == PickAssetPathWidget->ShowModal())
		{
			// Get the full name of where we want to create the physics asset.
			FString UserPackageName = PickAssetPathWidget->GetFullAssetPath().ToString();
			FName BPName(*FPackageName::GetLongPackageAssetName(UserPackageName));

			// Check if the user inputed a valid asset name, if they did not, give it the generated default name
			if (BPName == NAME_None)
			{
				// Use the defaults that were already generated.
				UserPackageName = PackageName;
				BPName = *Name;
			}

			// Then find/create it.
			UPackage* Package = CreatePackage(NULL, *UserPackageName);
			check(Package);

			// Create and init a new Blueprint
			Blueprint = FKismetEditorUtilities::CreateBlueprint(ParentClass, Package, BPName, BPTYPE_Normal, UGAEffectBlueprint::StaticClass(), UBlueprintGeneratedClass::StaticClass(), FName("LevelEditorActions"));
			if (Blueprint)
			{
				// Notify the asset registry
				FAssetRegistryModule::AssetCreated(Blueprint);

				// Mark the package dirty...
				Package->MarkPackageDirty();
			}
		}
		if (Blueprint != NULL && Blueprint->GeneratedClass )
		{
			UGAEffectBlueprint* eff = Cast<UGAEffectBlueprint>(Blueprint);
			if (eff)
			{
				EffectPropertyHandle->SetValueFromFormattedString(eff->GeneratedClass->GetPathName());

				FAssetEditorManager::Get().OpenEditorForAsset(eff);
				return FReply::Handled();
			}
		}
	}
	return FReply::Unhandled();
}

FReply FGAEffectClassStructWidget::OnEditButtonClicked()
{
	if (EffectEditorWindow.IsValid())
	{
		//EffectEditorWindow->RequestDestroyWindow();
		EffectEditorWindow->DestroyWindowImmediately();
		EffectEditorWindow.Reset();
		// already open, just show it
		//EffectEditorWindow->BringToFront(true);
	}
	
	{
		UObject* OutVal = nullptr;
		UGAEffectBlueprint* Blueprint = nullptr;
		UProperty* Prop;
		UBlueprintGeneratedClass* Class = nullptr;
		if (EffectPropertyHandle.IsValid())
		{
			EffectPropertyHandle->GetValue(OutVal);
			EffectPropertyHandle->CreatePropertyValueWidget();
			Prop = EffectPropertyHandle->GetProperty();
			
			if (OutVal)
			{
				Class = Cast<UBlueprintGeneratedClass>(OutVal);
				if (Class && Class->ClassGeneratedBy && Class->ClassGeneratedBy->IsA(UGAEffectBlueprint::StaticClass()))
				{
					Blueprint = Cast<UGAEffectBlueprint>(Class->ClassGeneratedBy);
				}
			}
		}

		FDetailsViewArgs Args;
		Args.bHideSelectionTip = true;
		Args.bLockable = false;
		FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		TSharedRef<IDetailsView> DetailView = PropertyEditorModule.CreateDetailView(Args);
		TArray<UObject*> InObjects;
		if (Blueprint)
		{
			EditedBlueprint = Blueprint;
			if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(UAFAbilityActivationSpec::StaticClass()))
			{
				DetailView->RegisterInstancedCustomPropertyLayout(UAFAbilityActivationSpec::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FAFAbilityActivationSpecDetails::MakeInstance));
			}
			else if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(UAFAbilityPeriodSpec::StaticClass()))
			{
				DetailView->RegisterInstancedCustomPropertyLayout(UAFAbilityPeriodSpec::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FAFAbilityPeriodSpecDetails::MakeInstance));
			}
			else if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(UAFAbilityCooldownSpec::StaticClass()))
			{
				DetailView->RegisterInstancedCustomPropertyLayout(UAFAbilityCooldownSpec::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FAFAbilityCooldownSpecDetails::MakeInstance));
			}
			else if (Blueprint->ParentClass && Blueprint->ParentClass->IsChildOf(UGAGameEffectSpec::StaticClass()))
			{
			//	DetailView->UnregisterInstancedCustomPropertyLayout(UGAGameEffectSpec::StaticClass());
			//	DetailView->RegisterInstancedCustomPropertyLayout(UGAGameEffectSpec::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FGAEffectDetails::MakeInstance));
			}
		}
		if(!Class)
		{
			return FReply::Unhandled();
		}
		if (!Class->ClassDefaultObject)
		{
			return FReply::Unhandled();
		}
		//DetailView->RegisterInstancedCustomPropertyLayout(UGAGameEffectSpec::StaticClass(), FOnGetDetailCustomizationInstance::CreateStatic(&FGAEffectDetails::MakeInstance));
		InObjects.Add(Class->ClassDefaultObject);
		DetailView->SetObjects(InObjects);
		FString WindowTitle = "Effect Editor: ";
		WindowTitle += Blueprint->GetName();
		EffectEditorWindow = SNew(SWindow)
			.Title(FText::FromString(WindowTitle))
			.HasCloseButton(true)
			.ClientSize(FVector2D(600, 400));
		EffectEditorWindow->SetContent(
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush(TEXT("PropertyWindow.WindowBorder")))
			[
				SNew(SVerticalBox)
				+SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(24.0f)
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SNew(SButton)
						.OnClicked(this, &FGAEffectClassStructWidget::OnSaveButtonClicked)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Save"))
						]
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.OnClicked(this, &FGAEffectClassStructWidget::OnSaveCloseButtonClicked)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Save/Close"))
						]
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SButton)
						.OnClicked(this, &FGAEffectClassStructWidget::OnCloseButtonClicked)
						.IsEnabled(true)
						.IsFocusable(false)
						[
							SNew(STextBlock)
							.Text(FText::FromString("Close"))
						]
					]
				]
				+SVerticalBox::Slot()
				.FillHeight(1)
				[
					DetailView
				]
			]
		);

		if (FGlobalTabmanager::Get()->GetRootWindow().IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(EffectEditorWindow.ToSharedRef(), FGlobalTabmanager::Get()->GetRootWindow().ToSharedRef());
		}
		else
		{
			FSlateApplication::Get().AddWindow(EffectEditorWindow.ToSharedRef());
		}
	}

	return FReply::Handled();
}
FReply FGAEffectClassStructWidget::OnSaveButtonClicked()
{
	if (EditedBlueprint)
	{
		FCompilerResultsLog LogResults;
		LogResults.BeginEvent(TEXT("Compile"));
		LogResults.bLogDetailedResults = false;
		LogResults.EventDisplayThresholdMs = 0.1;
		EBlueprintCompileOptions CompileOptions = EBlueprintCompileOptions::None;
		CompileOptions |= EBlueprintCompileOptions::SaveIntermediateProducts;

		FKismetEditorUtilities::CompileBlueprint(EditedBlueprint, CompileOptions, &LogResults);
		TArray<UPackage*> PackagesToSave;
		check((EditedBlueprint != nullptr) && EditedBlueprint->IsAsset());
		PackagesToSave.Add(EditedBlueprint->GetOutermost());

		FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, /*bPromptToSave=*/ false);
		//EditedBlueprint = nullptr;
	}
	return FReply::Unhandled();
}
FReply FGAEffectClassStructWidget::OnSaveCloseButtonClicked()
{
	if (EditedBlueprint)
	{
		FCompilerResultsLog LogResults;
		LogResults.BeginEvent(TEXT("Compile"));
		LogResults.bLogDetailedResults = false;
		LogResults.EventDisplayThresholdMs = 0.1;
		EBlueprintCompileOptions CompileOptions = EBlueprintCompileOptions::None;
		CompileOptions |= EBlueprintCompileOptions::SaveIntermediateProducts;

		FKismetEditorUtilities::CompileBlueprint(EditedBlueprint, CompileOptions, &LogResults);
		TArray<UPackage*> PackagesToSave;
		check((EditedBlueprint != nullptr) && EditedBlueprint->IsAsset());
		PackagesToSave.Add(EditedBlueprint->GetOutermost());

		FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, true, /*bPromptToSave=*/ false);

		EditedBlueprint = nullptr;

		if (EffectEditorWindow.IsValid())
		{
			EffectEditorWindow->RequestDestroyWindow();
			EffectEditorWindow = nullptr;
			
			return FReply::Handled();
		}
	}
	
	return FReply::Unhandled();
}
FReply FGAEffectClassStructWidget::OnCloseButtonClicked()
{
	if (EffectEditorWindow.IsValid())
	{
		EffectEditorWindow->RequestDestroyWindow();
		EffectEditorWindow = nullptr;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}