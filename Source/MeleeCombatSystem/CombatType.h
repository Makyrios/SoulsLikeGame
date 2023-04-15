// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECombatType : uint8
{
    None         UMETA(DisplayName = "None"),
    LightSword   UMETA(DisplayName = "LightSword"),
    GreatSword   UMETA(DisplayName = "GreatSword"),
    DualSword    UMETA(DisplayName = "DualSword")
};
