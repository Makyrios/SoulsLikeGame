// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EStats : uint8
{
	None	UMETA(DisplayName = "None"),
	Health	UMETA(DisplayName = "Health"),
	Stamina	UMETA(DisplayName = "Stamina"),
	Armor	UMETA(DisplayName = "Armor")
};

