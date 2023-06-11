// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "Combat.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UCombat : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MELEECOMBATSYSTEM_API ICombat
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void ContinueAttack(FGameplayTag AttackType) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ResetAttack() = 0;

	UFUNCTION(BlueprintCallable)
	virtual FRotator GetDesiredRotation() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void ResetCombat() = 0;

	UFUNCTION(BlueprintCallable)
	virtual bool CanReceiveDamage() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetInvincibleFrames(bool bEnableIFrames) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetHyperarmorFrames(bool bEnableHFrames) = 0;

	UFUNCTION(BlueprintCallable)
	virtual void UpdateHealthPotionAmount(int Amount) = 0;
};
