#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/Actor.h"
#include "ObjectPoolerSettings.h"
#include "Templates/Tuple.h"
#include "ObjectPooler.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogObjectPooler, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPoolerFinishedSpawningSignature);

UCLASS()
class SHRIMPLE_POOLING_API UObjectPooler : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	/*This event fires when object pooler finishes spawning all objects*/
	UPROPERTY(BlueprintAssignable)
	FOnPoolerFinishedSpawningSignature OnPoolerSpawnFinished;
	
	/*
	 Initialise a list of object poolers from a type
	 ActorClass - Spawnable class implementing the pooler interface
	 SpawnPoolTypes - Settings for the class to spawn
	 */
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
	void InitObjectPoolList(TMap<TSubclassOf<AActor> ,FPoolerOptions> SpawnPoolTypes);
	
	/* Creates a new pooled class at over a single frame*/
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
	bool SpawnPooledClass(TSubclassOf<AActor> Class, FPoolerOptions SpawnSettings);
	
	/*Activates an  actor from the object pooler and returns a reference to the object*/
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler", meta =(WorldContext = "WorldContextObject", DeterminesOutputType = "ObjectType"))
	AActor* GetObjectFromPool(TSubclassOf<AActor> ObjectType, bool& Success);
	
    /*Activates multiple actors from the object pooler and returns an array of objects*/
    UFUNCTION(BlueprintCallable, Category = "ObjectPooler", meta =(WorldContext = "WorldContextObject", DeterminesOutputType = "ObjectType"))
    TArray<AActor*> GetObjectsFromPool(TSubclassOf<AActor> ObjectType,int Count, bool& Success);
	
	/*Returns the selected actor to the object pooler*/
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
	bool ReturnObjectToPool(AActor* Object);

	/*Returns the selected actors to the object pooler*/
    UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
    void ReturnObjectsToPool(TArray<AActor*> Objects);

	/*Remove an object type from the pooler list*/
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
	bool ClearPoolOfType(TSubclassOf<AActor> ObjectType, bool ForceRemove = false);

	/*Set the cleanup timer*/
	UFUNCTION(BlueprintCallable, Category = "ObjectPooler")
	void SetCleanupTimer(bool InTimerActive);

protected:
	
	/*Remove null reference from specific pool class*/
	UFUNCTION(Category = "ObjectPooler")
	void CleanupPooledClass(TSubclassOf<AActor> Class);

	/* 
	 *Remove null references from all arrays - not recommended frequent use
	 *Advised use in load zones
	 */
	UFUNCTION(Category = "ObjectPooler")
	void CleanupAllPools();

	void CheckForCleanup();
	
	
	FTimerHandle CleanupTimerHandle;
	TSubclassOf<AActor> ClassToClean;
	//Pooler Options, Count & Expansion
	UPROPERTY(VisibleAnywhere, Category = "ObjectPooler")
	TMap<TSubclassOf<AActor>, FPoolerOptions> SettingsMap;
	
	/*MapKey - ClassType
	MapValue - ObjectPools
	TupleKey - ActivePooledObjects
	TupleValue - InactivePooledObjects*/
	TMap<TSubclassOf<AActor>, TTuple<TArray<AActor*>,TArray<AActor*>>> PooledObjects;

	
	//Create a new object internally
	UFUNCTION(BlueprintInternalUseOnly, Category = "ObjectPooler")
	AActor* SpawnNewObject(const TSubclassOf<AActor>& ObjectType) const;

	AActor* GetLastValid(const TArray<AActor*>& FromPool);
	void SwapObjectToPool(TArray<AActor*>& FromPool, TArray<AActor*>& ToPool, AActor* MoveObject);


	
private:

	UFUNCTION(BlueprintCallable)
	void DebugPooler();
	
	/*Removes nullptr references within a given threshold (Settings)*/
	void CleanupCheck(TArray<AActor*>& Pool,int Threshold);
	void CompactArray(TArray<AActor*>& Pool);

	/*Call the interface on the actor*/
	bool ActivateObject( AActor* Actor);
	bool DeactivateObject( AActor* Actor);
	
	/*Pooling Settings*/
	UPROPERTY()
	const UObjectPoolerSettings* PoolerSettings;
	
	UPROPERTY()
	UWorld* World;

};

