// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class ECollisionPart : uint8
{
	MainWeapon		UMETA(DisplayName = "Main Weapon"),
	OffHandWeapon	UMETA(DisplayName = "Off-hand Weapon"),
	RightFoot		UMETA(DisplayName = "Right Foot")
};

