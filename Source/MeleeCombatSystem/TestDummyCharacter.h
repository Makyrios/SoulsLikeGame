// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Targetable.h"

#include "CoreMinimal.h"
#include "MeleeCombatSystemCharacter.h"
#include "TestDummyCharacter.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ATestDummyCharacter : public AMeleeCombatSystemCharacter, public ITargetable
{
	GENERATED_BODY()

public:
	ATestDummyCharacter();

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
	class UWidgetComponent* LockOnWidgetComponent;

	// Inherited via ITargetable
	virtual bool CanBeTargeted() override;

	// Inherited via ITargetable
	virtual void OnTargeted(bool bIsTargeted) override;

};
