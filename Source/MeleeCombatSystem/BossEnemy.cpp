// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "BossHealthBar.h"
#include <Kismet/GameplayStatics.h>
#include "StateManager.h"

ABossEnemy::ABossEnemy()
{
}

void ABossEnemy::SetBossHealthVisibility(bool bIsVisible)
{
	if (BossHealthBarWidget != nullptr)
	{
		if (bIsVisible && StateManager->GetCurrentState() != FGameplayTag::RequestGameplayTag("Character.State.Dead"))
		{
			BossHealthBarWidget->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			BossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ABossEnemy::BeginPlay()
{
	Super::BeginPlay();

	BossHealthBarWidget = CreateWidget<UBossHealthBar>(UGameplayStatics::GetPlayerController(this, 0), BossHealthBarClass);
	if (BossHealthBarWidget)
	{
		BossHealthBarWidget->InitializeWidget(StatsComponent);
		BossHealthBarWidget->AddToViewport();
		BossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}
