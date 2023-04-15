// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ERotationMode : uint8
{
	OrientToCamera		UMETA(DisplayName = "Orient To Camera"),
	OrientToMovement	UMETA(DisplayName = "Orient To Movement")
};
