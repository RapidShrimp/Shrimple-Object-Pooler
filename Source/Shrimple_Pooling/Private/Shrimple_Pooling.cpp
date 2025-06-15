// Copyright Epic Games, Inc. All Rights Reserved.

#include "Shrimple_Pooling.h"

#define LOCTEXT_NAMESPACE "FShrimple_PoolingModule"

void FShrimple_PoolingModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FShrimple_PoolingModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FShrimple_PoolingModule, Shrimple_Pooling)