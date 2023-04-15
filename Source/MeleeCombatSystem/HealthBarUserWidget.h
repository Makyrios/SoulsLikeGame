// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EStats.h"
#include "HealthBarUserWidget.generated.h"

class UStatsComponent;
class UProgressBar;

UCLASS()
class MELEECOMBATSYSTEM_API UHealthBarUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

	void InitializeWidget(UStatsComponent* Stats);

private:
	UStatsComponent* StatsComponent;

	UPROPERTY(EditAnywhere, meta = (BindWidget))
	UProgressBar* StatBar;

	UFUNCTION()
	void OnCurrentStateValueUpdate(EStats StatType, float Value);
};
