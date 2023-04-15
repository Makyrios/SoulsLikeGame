// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Targetable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UTargetable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MELEECOMBATSYSTEM_API ITargetable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool CanBeTargeted() = 0;

	virtual void OnTargeted(bool bIsTargeted) = 0;
};
