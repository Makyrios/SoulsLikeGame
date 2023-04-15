// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Consumable.h"
#include "HealthPotion.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AHealthPotion : public AConsumable
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
