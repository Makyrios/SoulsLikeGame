// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterAI.h"
#include "BossEnemy.generated.h"

class UBossHealthBar;

UCLASS()
class MELEECOMBATSYSTEM_API ABossEnemy : public AMasterAI
{
	GENERATED_BODY()

public:
	ABossEnemy();

	void SetBossHealthVisibility(bool bIsVisible);
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UBossHealthBar> BossHealthBarClass;

	UBossHealthBar* BossHealthBarWidget;
};
