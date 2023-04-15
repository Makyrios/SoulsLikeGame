// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ECharacterAction : uint8
{
	Nothing			UMETA(DisplayName = "Nothing"),
	LightAttack		UMETA(DisplayName = "Light Attack"),
	StrongAttack	UMETA(DisplayName = "Strong Attack"),
	ChargedAttack	UMETA(DisplayName = "Charged Attack"),
	FallingAttack	UMETA(DisplayName = "Falling Attack"),
	SprintAttack	UMETA(DisplayName = "Sprint Attack"),
	Dodge			UMETA(DisplayName = "Dodge"),
	EnterCombat		UMETA(DisplayName = "Enter Combat"),
	ExitCombat		UMETA(DisplayName = "Exit Combat"),
	GeneralAction	UMETA(DispalyName = "General Action")
};
