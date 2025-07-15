// Fill out your copyright notice in the Description page of Project Settings.


#include "ObjectPooler.h"
#include "PooledObject.h"
#include "Kismet/KismetSystemLibrary.h"

DEFINE_LOG_CATEGORY(LogObjectPooler);

void UObjectPooler::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	World = GetWorld();
	PoolerSettings = GetDefault<UObjectPoolerSettings>();

	
	UE_LOG(LogObjectPooler,Display,TEXT("Started Object Pooler Subsystem"))
	
}

void UObjectPooler::Deinitialize()
{
	//Clear References
	SettingsMap.Empty();
	PooledObjects.Empty();
	Super::Deinitialize();
	UE_LOG(LogObjectPooler,Display,TEXT("Ending Object Pooler Subsystem"))

}

void UObjectPooler::InitObjectPoolList(TMap<TSubclassOf<AActor>, FPoolerOptions> SpawnPoolTypes)
{
	TArray<TSubclassOf<AActor>> ClassList;
	SpawnPoolTypes.GetKeys(ClassList);
	
	//Spawn every class at the start all at once - Slow but clears overhead later
	for (TSubclassOf<AActor> ActorClass : ClassList)
	{
		FPoolerOptions PoolerOptions = SpawnPoolTypes[ActorClass];
		if(!SpawnPooledClass(ActorClass,PoolerOptions))
		{
			UE_LOG(LogObjectPooler,Error,TEXT("Failed to initialise class '%s'"),*ActorClass->GetName());
			SettingsMap.Remove(ActorClass);	
		}
	}
	SetCleanupTimer(true);
	OnPoolerSpawnFinished.Broadcast();
	UE_LOG(LogObjectPooler,Display,TEXT("Finished all object pooler spawning class at once"));
	
}

bool UObjectPooler::SpawnPooledClass(TSubclassOf<AActor> Class, FPoolerOptions SpawnSettings)
{
	if(!IsValid(Class)){return false;}
	if(!UKismetSystemLibrary::DoesClassImplementInterface(Class,UPooledObject::StaticClass()))
	{
		UE_LOG(LogObjectPooler,Error,TEXT("Class '%s' does not implement IPooledObject"),*Class->GetName());
		return false;
	}

	if(PooledObjects.Contains(Class))
	{
		UE_LOG(LogObjectPooler,Error,TEXT("Class '%s' already exists"), *Class->GetName());
	
		return false;
	}
	TArray<AActor*> InactivePool;
	InactivePool.Reserve(SpawnSettings.PreSpawnCount);
	
	SettingsMap.Add(Class,SpawnSettings);

	for(int i = 0; i < SpawnSettings.PreSpawnCount; i++)
	{
		AActor* NewObject = SpawnNewObject(Class);
		if(!NewObject) { continue;}

		InactivePool.Add(NewObject);
		DeactivateObject(NewObject);
	}
	PooledObjects.Add(Class,TTuple<TArray<AActor*>,TArray<AActor*>>(TArray<AActor*>(),InactivePool));
	UE_LOG(LogObjectPooler,Display,TEXT("Finished spawning class '%s' at once"),*Class->GetName());

	return true;
}

AActor* UObjectPooler::SpawnNewObject(const TSubclassOf<AActor>& ObjectType) const
{
	if (!World) { return nullptr; }

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bDeferConstruction = false;

	AActor* NewActor = World->SpawnActor<AActor>(ObjectType.Get(), FVector(0, 0, 0), FRotator(0, 0, 0), Params);
	#if WITH_EDITOR
	NewActor->SetFolderPath("/Pool");
	#endif
	

	if (!IsValid(NewActor))
	{
		UE_LOG(LogObjectPooler, Error, TEXT("Failed to spawn actor of class '%s'"), *ObjectType->GetName())
		return nullptr;
	}

	return NewActor;
	
}

AActor* UObjectPooler::GetObjectFromPool(TSubclassOf<AActor> ObjectType, bool& Success)
{
	TTuple<TArray<AActor*>,TArray<AActor*>>* Pool = PooledObjects.Find(ObjectType);
	if(!Pool)
	{
		UE_LOG(LogObjectPooler,Error,TEXT("Cannot activate '%s' is not a pooler initialised type\nCall SpawnPooledClass() with the class type to initialise"),*ObjectType->GetName());
		Success = false;
		return nullptr;
	}
	
	AActor* OutObject = GetLastValid(Pool->Value);
	if(OutObject)
	{
		SwapObjectToPool(Pool->Value,Pool->Key,OutObject);
		Success = ActivateObject(OutObject);
		return OutObject;		
	}
	
	//Fallback to empty pool settings
	const FPoolerOptions* PoolSettings = SettingsMap.Find(ObjectType);
	if(!PoolSettings)
	{
		UE_LOG(LogObjectPooler,Warning,TEXT("No pooler settings found for '%s'"),*ObjectType->GetName());
		Success = false;
		return nullptr;
	} 
	
	if(PoolSettings->bExpandable)
	{
		OutObject = SpawnNewObject(ObjectType);
		Pool->Key.Add(OutObject);
		Success = ActivateObject(OutObject);
		return OutObject;
	}
	if(PoolSettings->bFindOldest)
	{
		//Remove and reassign the object to the end of the array
		OutObject = Pool->Key[0];
		SwapObjectToPool(Pool->Key,Pool->Key,OutObject);
		Success = ActivateObject(OutObject);
		return OutObject;
	}

	Success = false;
	return nullptr;
}

TArray<AActor*> UObjectPooler::GetObjectsFromPool(TSubclassOf<AActor> ObjectType, int Count, bool& Success)
{
	TArray<AActor*> OutArray;
	if(!PooledObjects.Find(ObjectType))
	{
		UE_LOG(LogObjectPooler,Error,TEXT("Cannot activate '%s' is not a pooler initialised type\nCall SpawnPooledClass() with the class type to initialise"),*ObjectType->GetName());

		return OutArray; 
	}
	
	for(int i = 0; i < Count; i++)
	{
		OutArray.Add(GetObjectFromPool(ObjectType,Success));
	}

	return OutArray;
}

bool UObjectPooler::ReturnObjectToPool(AActor* Object)
{
	if(!Object)
	{
		UE_LOG(LogObjectPooler,Warning,TEXT("No object to add to pool"));
		return false;
	}

	if(!UKismetSystemLibrary::DoesImplementInterface(Object,UPooledObject::StaticClass()))
	{
		UE_LOG(LogObjectPooler,Warning,TEXT("Actor does not implement IPooledObject"));
		return false;
	}

	if(!PooledObjects.Contains(Object->GetClass()))
	{
		UE_LOG(LogObjectPooler,Warning,TEXT("This object isnt of an initailised pool type"));
		return false;
	}

	//Move from Active pool (The Key) to the inactive pool, (Value)
	TTuple<TArray<AActor*>,TArray<AActor*>>* Pool = PooledObjects.Find(Object->GetClass());
	SwapObjectToPool(Pool->Key,Pool->Value,Object);
	DeactivateObject(Object);
	return true;
}

void UObjectPooler::ReturnObjectsToPool(TArray<AActor*> Objects)
{
	for (AActor* Object : Objects)
	{
		if(!IsValid(Object))
		{
			UE_LOG(LogObjectPooler,Warning,TEXT("Nullptr found in parsed object array"));
			continue;
		}

		if(!PooledObjects.Find(Object->GetClass()))
		{
			UE_LOG(LogObjectPooler,Warning,TEXT("%s is not of an initialised pool type"),*Object->GetClass()->GetName());
			continue;
		}

		ReturnObjectToPool(Object);
	}
	
}

bool UObjectPooler::ActivateObject(AActor* Actor)
{
	if(!Actor) {return false;}
	Actor->SetActorHiddenInGame(false);
	Actor->SetActorTickEnabled(true);
	Actor->SetActorEnableCollision(true);
	IPooledObject::Execute_OnActivated(Actor);
	return true;
}

bool UObjectPooler::DeactivateObject(AActor* Actor)
{
	if(!Actor) {return false;}
	Actor->SetActorHiddenInGame(true);
	Actor->SetActorTickEnabled(false);
	Actor->SetActorEnableCollision(false);
	IPooledObject::Execute_OnDeactivated(Actor);
	return true;
}

AActor* UObjectPooler::GetLastValid(const TArray<AActor*>& FromPool)
{
	for (int32 i = FromPool.Num() - 1; i >= 0; --i)
	{
		AActor* ObjectSlot = FromPool[i];
		if (!IsValid(ObjectSlot)) { continue; }

		return ObjectSlot;
	}
	return nullptr; //No object was found
}

void UObjectPooler::SwapObjectToPool(TArray<AActor*>& FromPool, TArray<AActor*>& ToPool, AActor* MoveObject)
{
	//Null ref to prevent array shuffling via .remove()
	for (int32 i = 0; i < FromPool.Num(); i++)
	{
		if (FromPool[i] == MoveObject)
		{
			FromPool[i] = nullptr;
			break;
		}
	}

	//Add ref to target array reusing null slot if any
	for (int32 i = 0; i < ToPool.Num(); i++)
	{
		if (ToPool[i] == nullptr || !IsValid(ToPool[i]))
		{
			ToPool[i] = MoveObject;
			return;
		}
	}

	//No Null slot found
	ToPool.Add(MoveObject);
}


bool UObjectPooler::ClearPoolOfType(TSubclassOf<AActor> ObjectType, bool ForceRemove)
{
	TTuple<TArray<AActor*>,TArray<AActor*>>* Pool = PooledObjects.Find(ObjectType);
	
	if(!Pool)
	{
		UE_LOG(LogObjectPooler,Warning,TEXT("Couldn't remove class '%s' from pooler as none of type exists"),*ObjectType->GetName());
		return false;
	}
	
	FPoolerOptions* Settings = SettingsMap.Find(ObjectType);

	//Settings Exist -> Does it have destroy on removal
	//Settings are missing -> Safety destroy
	//Force Removal
	if((Settings && Settings->bDestroyOnPoolRemoval) || !Settings || ForceRemove)
	{
		for (AActor* ActiveObject : Pool->Key)
		{
			ActiveObject->Destroy();
		}
		for (AActor* InactiveObject : Pool->Value)
		{
			InactiveObject->Destroy();
		}
	}
	PooledObjects.Remove(ObjectType);
	SettingsMap.Remove(ObjectType);
	return true;
}

void UObjectPooler::SetCleanupTimer(bool InTimerActive)
{
	if(InTimerActive)
	{
		const float UpdateTime = PoolerSettings ? PoolerSettings->CleanupCheckRate : 5.0f;
		GetWorld()->GetTimerManager().SetTimer(CleanupTimerHandle,this,&UObjectPooler::CheckForCleanup,UpdateTime,true);
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(CleanupTimerHandle);
	}
}



void UObjectPooler::CheckForCleanup()
{
	const float CurrentTime = World->GetTimeSeconds();

	//Loop through all pool settings
	for (TTuple<TSubclassOf<AActor>, FPoolerOptions>& PoolSettings : SettingsMap)
	{
		const TSubclassOf<AActor> ClassType = PoolSettings.Key;
		FPoolerOptions& Settings = PoolSettings.Value;

		//Check Settings for cleanup time
		if (CurrentTime - Settings.LastCleanupTime >= Settings.CleanupTime)
		{
			CleanupPooledClass(ClassType);
			Settings.LastCleanupTime = CurrentTime;
		}
	}
}

void UObjectPooler::CleanupAllPools()
{
	TArray<TSubclassOf<AActor>> ClassList;
	PooledObjects.GetKeys(ClassList);

	if(ClassList.IsEmpty()) {return;}

	for (TSubclassOf<AActor> Class : ClassList)
	{
		CleanupPooledClass(Class);
	}
}

void UObjectPooler::CleanupPooledClass(TSubclassOf<AActor> Class)
{
	if(!IsValid(Class)) {return;}

	TTuple<TArray<AActor*>,TArray<AActor*>>* Pools = PooledObjects.Find(Class);
	
	FPoolerOptions* Settings = SettingsMap.Find(Class);
	int CleanupThreshold = Settings ? Settings->CleanupThreshold : 5;

	CleanupCheck(Pools->Key,CleanupThreshold);
	CleanupCheck(Pools->Value,CleanupThreshold);
	
}

void UObjectPooler::CleanupCheck(TArray<AActor*>& Pool, int Threshold)
{
	int NullCount = 0;

	for (AActor* Object : Pool)
	{
		if (Object != nullptr || IsValid(Object)) {continue;}

		NullCount++;
		if (NullCount < Threshold) { continue; }

		CompactArray(Pool);
		break;
	}
}

void UObjectPooler::CompactArray(TArray<AActor*>& Pool)
{
	// Manually remove null or invalid pointers from Pool
	for (int32 i = Pool.Num() - 1; i >= 0; --i)
	{
		if (Pool[i] == nullptr || !IsValid(Pool[i]))
		{
			Pool.RemoveAt(i);
		}
	}
}

void UObjectPooler::DebugPooler()
{
	for (TTuple<TSubclassOf<AActor>, TTuple<TArray<AActor*>, TArray<AActor*>>> Pool : PooledObjects) 
	{
		if(!Pool.Key){continue;}
		UE_LOG(LogObjectPooler,Display,TEXT(""));
		UE_LOG(LogObjectPooler,Display,TEXT("Pool: %s | Active: %d | Inactive: %d"),*Pool.Key->GetName(),Pool.Value.Key.Num(),Pool.Value.Value.Num());
		
		FPoolerOptions* Setting = SettingsMap.Find(Pool.Key);
		if(!Setting)
		{
			UE_LOG(LogObjectPooler,Warning,TEXT("No Pooler Settings!"));
			continue;
		}
		
		UE_LOG(LogObjectPooler,Display,TEXT("PoolSettings: %s | Can Expand |"),*Pool.Key->GetName());

	}
}

