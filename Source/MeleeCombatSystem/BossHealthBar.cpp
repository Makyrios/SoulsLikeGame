// Fill out your copyright notice in the Description page of Project Settings.


#include "BossHealthBar.h"
#include <Components/CanvasPanel.h>
#include "StatsComponent.h"
#include "Components/ProgressBar.h"


void UBossHealthBar::InitializeWidget(UStatsComponent* Stats)
{ 
	if (Stats != nullptr)
	{
		Stats->OnCurrentStatValueUpdated.AddDynamic(this, &UBossHealthBar::OnCurrentStateValueUpdate);
		StatsComponent = Stats; 
		OnCurrentStateValueUpdate(EStats::Health, StatsComponent->GetCurrentStatValue(EStats::Health));
	}
}


void UBossHealthBar::OnCurrentStateValueUpdate(EStats Stat, float Value)
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
