#include "WALandscapeGraphFactory.h"
#include "WALandscapeGraph.h"
#define LOCTEXT_NAMESPACE "GenericGraph"

UWALandscapeGraphFactory::UWALandscapeGraphFactory()
{
	bCreateNew = true;
	bEditAfterNew = true;
	SupportedClass = UWALandscapeGraph::StaticClass();
}

UWALandscapeGraphFactory::~UWALandscapeGraphFactory()
{

}

UObject* UWALandscapeGraphFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UObject>(InParent, Class, Name, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE
