// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EStats.h"
#include "BossHealthBar.generated.h"

class UCanvasPanel;
class UProgressBar;
class UStatsComponent;

UCLASS()
class MELEECOMBATSYSTEM_API UBossHealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeWidget(UStatsComponent* Stats);

private:
	UStatsComponent* StatsComponent;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UCanvasPanel* CanvasPanel;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* StatBar;
	

	UFUNCTION()
	void OnCurrentStateValueUpdate(EStats Stat, float Value);
};
