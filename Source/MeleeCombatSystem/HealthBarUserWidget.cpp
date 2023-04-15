// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarUserWidget.h"
#include "Components/ProgressBar.h"
#include "StatsComponent.h"
#include "EStats.h"

void UHealthBarUserWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UHealthBarUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHealthBarUserWidget::InitializeWidget(UStatsComponent* Stats)
{
	if (Stats != nullptr)
	{
		Stats->OnCurrentStatValueUpdated.AddDynamic(this, &UHealthBarUserWidget::OnCurrentStateValueUpdate);
		StatsComponent = Stats;
		OnCurrentStateValueUpdate(EStats::Health, StatsComponent->GetCurrentStatValue(EStats::Health));
	}
}


void UHealthBarUserWidget::OnCurrentStateValueUpdate(EStats Stat, float Value)
{
	if (Stat == EStats::Health)
	{
		if (StatsComponent == nullptr || StatBar == nullptr)
		{
			return;
		}
		StatBar->SetPercent(Value / StatsComponent->GetMaxStatValue(Stat));
	}
}
