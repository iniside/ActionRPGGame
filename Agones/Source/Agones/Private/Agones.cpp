// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "IAgones.h"

#if ENABLE_AGONES
#include "grpc/support/time.h"
#include "grpc++/grpc++.h"
#include "sdk.h"
#endif

class FAgones : public IAgones
{
#if ENABLE_AGONES

	agones::SDK* AgonesSDK;
#endif
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

IMPLEMENT_MODULE( FAgones, Agones)



void FAgones::StartupModule()
{
#if ENABLE_AGONES
	
	grpc::string out = grpc::Version();
	FString ver = FString(ANSI_TO_TCHAR(out.c_str()));
	UE_LOG(LogTemp, Warning, TEXT("FGRPC::StartupModule Version: %s "), *ver);
	AgonesSDK = new agones::SDK();
#endif
}


void FAgones::ShutdownModule()
{
#if ENABLE_AGONES
	//if (AgonesSDK)
	//{
	//	delete AgonesSDK;
	//	AgonesSDK = nullptr;
	//}
#endif
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}
