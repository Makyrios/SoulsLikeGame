// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "EDamageType.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EDamageType : uint8
{
	None			UMETA(DisplayName = "None"),
	MeleeDamage		UMETA(DisplayName = "Melee Damage"),
	KnockDownDamage	UMETA(DisplayName = "Knock Down Damage")
};
