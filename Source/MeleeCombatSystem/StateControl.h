// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ECharacterState.h"
#include "ECharacterAction.h"
#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"
#include "EMovementSpeedMode.h"
#include "StateControl.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UStateControl : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MELEECOMBATSYSTEM_API IStateControl
{
	GENERATED_BODY()

public:
	virtual bool CanPerformAnimation() const = 0;

	// Character state
	virtual FGameplayTag GetCurrentState() const = 0;
	virtual void SetCurrentState(FGameplayTag NewState) = 0;
	virtual void ResetState() = 0;

	// Character action
	virtual FGameplayTag GetCurrentAction() const = 0;
	virtual void SetCurrentAction(FGameplayTag NewAction) = 0;
	virtual void ResetAction() = 0;
	virtual TArray<UAnimMontage*> GetMontageByTag(FGameplayTag Action) const = 0;

	virtual EMovementSpeedMode GetMovementSpeedMode() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual float PerformAction(FGameplayTag ActionTag, FGameplayTag StateTag, bool bRandomIndex = false, int MontageIndex = 0) = 0;

	UFUNCTION(BlueprintCallable)
	virtual float PerformCustomAction(FGameplayTag ActionTag, FGameplayTag StateTag, UAnimMontage* InMontage, float MontagePlayRate = 1.0f, bool bAutoReset = false) = 0;

	UFUNCTION(BlueprintCallable)
	virtual float PerformAttack(FGameplayTag AttackTag, int MontageIndex, bool bRandomIndex) = 0;
};
