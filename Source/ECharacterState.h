// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	Nothing				UMETA(DisplayName = "Nothing"),
	Attacking			UMETA(DisplayName = "Attacking"),
	Dodging				UMETA(DisplayName = "Dodging"),
	GeneralActionState	UMETA(DisplayName = "General Action State"),
	Dead				UMETA(DisplayName = "Dead"),
	Disabled			UMETA(DisplayName = "Disabled"),
	TogglingCombat		UMETA(DisplayName = "Toggling Combat")
};
