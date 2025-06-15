// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PooledObject.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPooledObject : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHRIMPLE_POOLING_API IPooledObject
{
	GENERATED_BODY()

public:

	//When the actor is returned to the object pool this function will run
	//This acts as an OnDeactivate function
	UFUNCTION(BlueprintImplementableEvent, Category="Object Pooler",meta = (Keywords = "Pooler Object ObjectPooler Deactivate"))
	void OnDeactivated();

	//When the actor is taken from the object pool this function will run
	//This acts as an OnActivate function
	UFUNCTION(BlueprintImplementableEvent, Category="Object Pooler",meta = (Keywords = "Pooler Object ObjectPooler Activate"))
	void OnActivated();

	
};
