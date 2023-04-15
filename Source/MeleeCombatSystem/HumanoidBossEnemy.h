// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossEnemy.h"
#include "HumanoidBossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AHumanoidBossEnemy : public ABossEnemy
{
	GENERATED_BODY()

public:
	AHumanoidBossEnemy();

	virtual void OnTargeted(bool bIsTargeted) override;

protected:
	void BeginPlay() override;

private:
	
};
