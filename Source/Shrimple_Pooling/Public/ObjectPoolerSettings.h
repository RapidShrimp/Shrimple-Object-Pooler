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
	//The number of actors to spawn on initialisation
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	int PreSpawnCount = 0;

	/*//Time in seconds that objects will spawn in
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	float SpawnRate = 0.0f;*/

	//If this is true more objects are spawnable into the pool when none are available
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	bool bExpandable = false;

	//Recycles the oldest active pooled object when the pool is not expandable
	//If set to false, no object will be returned
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	bool bFindOldest = false;

	//If the object type is removed from the pool, remove the 
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Spawning")
	bool bDestroyOnPoolRemoval = false;
	
	//If the object type is removed from the pool
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "Cleanup")
	int CleanupThreshold = 5;

	//The time between auto cleanup checks on the pool
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Cleanup")
	float CleanupTime = 10.0f;
	
	//Internal Property used for cleanup operations
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
	
	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly)
	bool bAutoCleanup = true;

	UPROPERTY(Config,EditAnywhere,BlueprintReadOnly)
	float CleanupCheckRate = 10.0f;

	

};
