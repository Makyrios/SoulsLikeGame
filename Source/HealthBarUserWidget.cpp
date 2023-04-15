// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarUserWidget.h"
#include "Components/ProgressBar.h"
#include "StatsComponent.h"

void UHealthBarUserWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

void UHealthBarUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UHealthBarUserWidget::InitializeWidget(UStatsComponent* Stats, EStats StatType)
{
	/*if (Stats == nullptr) return;
	StatsComponent = Stats;

	StatBar->SetWidgetStyle(Style);
	StatBar->SetFillColorAndOpacity(FillColor);

	StatsComponent->OnCurrentStatValueUpdated.AddDynamic(this, &UHealthBarUserWidget::OnCurrentStateValueUpdate);

	OnCurrentStateValueUpdate(StatType, Stats->GetCurrentStatValue(StatType));*/
}


void UHealthBarUserWidget::OnCurrentStateValueUpdate(EStats Stat, float Value)
{
	if (Stat == EStats::Health)
	{
		if (StatsComponent == nullptr || StatBar == nullptr) return;

		StatBar->SetPercent(Value / StatsComponent->GetMaxStatValue(Stat));
	}
}
