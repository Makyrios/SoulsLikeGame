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
	UE_LOG(LogTemp, Display, TEXT("Widget: %s"), *GetName());
	if (Stats != nullptr)
	{
		Stats->OnCurrentStatValueUpdated.AddDynamic(this, &UHealthBarUserWidget::OnCurrentStateValueUpdate);
		StatsComponent = Stats;
		OnCurrentStateValueUpdate(EStats::Health, StatsComponent->GetCurrentStatValue(EStats::Health));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Widget is not initialized. StatsComponent is null"));
	}
}


void UHealthBarUserWidget::OnCurrentStateValueUpdate(EStats Stat, float Value)
{
	UE_LOG(LogTemp, Display, TEXT("%s"), *StatsComponent->GetOwner()->GetActorNameOrLabel());
	if (Stat == EStats::Health)
	{
		if (StatsComponent == nullptr || StatBar == nullptr)
		{
			return;
		}
		StatBar->SetPercent(Value / StatsComponent->GetMaxStatValue(Stat));
	}
}
