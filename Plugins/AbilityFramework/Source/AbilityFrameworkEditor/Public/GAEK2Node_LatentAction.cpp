
#include "AbilityFrameworkEditor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "KismetCompiler.h"
#include "BlueprintEditorUtils.h"
#include "GAEK2Node_LatentAction.h"
#include "K2Node_EnumLiteral.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "LatentActions/AFTaskBase.h"
#define LOCTEXT_NAMESPACE "K2Node"

UGAEK2Node_LatentAction::UGAEK2Node_LatentAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyActivateFunctionName = GET_FUNCTION_NAME_CHECKED(UAFTaskBase, ReadyForActivation);
}

bool UGAEK2Node_LatentAction::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = true;

	//EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	//bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);

	//if (bAllowLatentFuncs)
	//{
	//	UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
	//	if (MyBlueprint && MyBlueprint->GeneratedClass)
	//	{
	//		if (MyBlueprint->GeneratedClass->IsChildOf(UGAAbilityBase::StaticClass()))
	//		{
	//			bIsCompatible = true;
	//		}
	//	}
	//}
	return bIsCompatible;
}


void UGAEK2Node_LatentAction::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//UK2Node_BaseAsyncTask::GetMenuActions(ActionRegistrar);
	struct GetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			UGAEK2Node_LatentAction* AsyncTaskNode = CastChecked<UGAEK2Node_LatentAction>(NewNode);
			if (FunctionPtr.IsValid())
			{
				UFunction* Func = FunctionPtr.Get();
				UObjectProperty* ReturnProp = CastChecked<UObjectProperty>(Func->GetReturnProperty());
						
				AsyncTaskNode->ProxyFactoryFunctionName = Func->GetFName();
				AsyncTaskNode->ProxyFactoryClass        = Func->GetOuterUClass();
				AsyncTaskNode->ProxyClass               = ReturnProp->PropertyClass;
			}
		}
	};

	UClass* NodeClass = GetClass();
	//FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateUObject(this, &UGAEK2Node_LatentAbilityTaskCall::CreateNodeSpawner);
	ActionRegistrar.RegisterClassFactoryActions<UAFTaskBase>(FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
	{
		UBlueprintNodeSpawner* NodeSpawner = UBlueprintFunctionNodeSpawner::Create(FactoryFunc);
		check(NodeSpawner != nullptr);
		NodeSpawner->NodeClass = NodeClass;

		TWeakObjectPtr<UFunction> FunctionPtr = const_cast<UFunction*>(FactoryFunc);
		NodeSpawner->CustomizeNodeDelegate = UBlueprintNodeSpawner::FCustomizeNodeDelegate::CreateStatic(GetMenuActions_Utils::SetNodeFunc, FunctionPtr);
		return NodeSpawner;
	}));
}

#undef LOCTEXT_NAMESPACE
