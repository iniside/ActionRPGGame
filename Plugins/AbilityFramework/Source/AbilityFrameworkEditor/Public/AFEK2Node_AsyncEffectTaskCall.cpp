
#include "AbilityFrameworkEditor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetArrayLibrary.h"
#include "LatentActions/AFTaskBase.h"
#include "Effects/EffectTasks/AFEffectTask.h"
#include "Effects/GAEffectExtension.h"
#include "KismetCompiler.h"
#include "BlueprintEditorUtils.h"
#include "AFEK2Node_AsyncEffectTaskCall.h"
#include "K2Node_EnumLiteral.h"
#include "BlueprintFunctionNodeSpawner.h"
#include "BlueprintActionDatabaseRegistrar.h"

#define LOCTEXT_NAMESPACE "K2Node"

UAFEK2Node_AsyncEffectTaskCall::UAFEK2Node_AsyncEffectTaskCall(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ProxyActivateFunctionName = GET_FUNCTION_NAME_CHECKED(UAFTaskBase, ReadyForActivation);
}

bool UAFEK2Node_AsyncEffectTaskCall::IsCompatibleWithGraph(UEdGraph const* TargetGraph) const
{
	bool bIsCompatible = false;

	EGraphType GraphType = TargetGraph->GetSchema()->GetGraphType(TargetGraph);
	bool const bAllowLatentFuncs = (GraphType == GT_Ubergraph || GraphType == GT_Macro);

	if (bAllowLatentFuncs)
	{
		UBlueprint* MyBlueprint = FBlueprintEditorUtils::FindBlueprintForGraph(TargetGraph);
		if (MyBlueprint && MyBlueprint->GeneratedClass)
		{
			if (MyBlueprint->GeneratedClass->IsChildOf(UGAEffectExtension::StaticClass()))
			{
				bIsCompatible = true;
			}
		}
	}
	return bIsCompatible;
}


void UAFEK2Node_AsyncEffectTaskCall::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//UK2Node_BaseAsyncTask::GetMenuActions(ActionRegistrar);
	struct GetMenuActions_Utils
	{
		static void SetNodeFunc(UEdGraphNode* NewNode, bool /*bIsTemplateNode*/, TWeakObjectPtr<UFunction> FunctionPtr)
		{
			UAFEK2Node_AsyncEffectTaskCall* AsyncTaskNode = CastChecked<UAFEK2Node_AsyncEffectTaskCall>(NewNode);
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
	ActionRegistrar.RegisterClassFactoryActions<UAFEffectTask>(FBlueprintActionDatabaseRegistrar::FMakeFuncSpawnerDelegate::CreateLambda([NodeClass](const UFunction* FactoryFunc)->UBlueprintNodeSpawner*
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
