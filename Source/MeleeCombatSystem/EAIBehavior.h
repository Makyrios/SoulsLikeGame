// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAIBehavior : uint8
{
	Nothing		UMETA(DisplayName = "Nothing"),
	Attack		UMETA(DisplayName = "Attack"),
	Chase		UMETA(DisplayName = "Chase"),
	Patrol		UMETA(DisplayName = "Patrol"),
	Hit			UMETA(DisplayName = "Hit")
};
