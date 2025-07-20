// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "ObjectPoolerSettings.generated.h"

USTRUCT(Blueprintable,BlueprintType)
struct FPoolerOptions
{
	GENERATED_BODY()
public:
	/*The number of actors to spawn on initialisation*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	const int PreSpawnCount = 0;

	/*If this is set true, more objects spawn into the pool when none are available*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	const bool bExpandable = false;

	/*
	 *Recycles the oldest active pooled object when the pool is not expandable
	 *If set to false, no object will be returned
	 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	const bool bFindOldest = false;

	/*If set true, when the object type is removed from the pool, destroy all referenced actors */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	const bool bDestroyOnPoolRemoval = false;
	
	/*Number of null spaces in a pool before cleanup is required*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Cleanup")
	const int CleanupThreshold = 5;

	/*The time between auto cleanup checks on the pool*/
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Cleanup")
	const float CleanupTime = 10.0f;
	
	/*Internal Property used for cleanup operations*/
	float LastCleanupTime = 0.0f;
};

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Shrimple Object Pooling"))
class SHRIMPLE_POOLING_API UObjectPoolerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

	UObjectPoolerSettings() {};

public:

	virtual FName GetCategoryName() const override { return TEXT("Plugins"); };
	virtual FName GetContainerName() const override { return TEXT("Project"); };
	virtual FName GetSectionName() const override { return TEXT("ObjectPooling"); };

	/* Automatically cleanup null refs in arrays after time
	 * *NOTE: Turning this off will cause memory problems on larger pooling sizes
	 */
	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly)
	bool bAutoCleanup = true;

	/*The frequency that the cleanup checks are done*/
	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly)
	float CleanupCheckRate = 10.0f;

	

};
