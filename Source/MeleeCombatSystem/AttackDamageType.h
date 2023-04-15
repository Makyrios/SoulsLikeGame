// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "EDamageType.h"
#include "AttackDamageType.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API UAttackDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UAttackDamageType();

	FORCEINLINE EDamageType GetDamageType() const { return DamageType; }
	FORCEINLINE void SetDamageType(EDamageType Value) { DamageType = Value; }
	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	EDamageType DamageType;
};
