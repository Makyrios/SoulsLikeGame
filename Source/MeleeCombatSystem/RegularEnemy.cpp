// Fill out your copyright notice in the Description page of Project Settings.


#include "RegularEnemy.h"
#include "Components/WidgetComponent.h"
#include <Kismet/GameplayStatics.h>
#include "HealthBarUserWidget.h"

ARegularEnemy::ARegularEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	// Health Bar Widget
	HealthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("Health Bar Widget Component"));
	HealthBarWidgetComponent->SetupAttachment(RootComponent);
	HealthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);

	//HealthBarWidgetComponent->SetWidgetClass(Widget);
}

void ARegularEnemy::OnTargeted(bool bIsTargeted)
{
	Super::OnTargeted(bIsTargeted);

	HealthBarWidgetComponent->SetVisibility(bIsTargeted);
}

void ARegularEnemy::BeginPlay()
{
	Super::BeginPlay();

	UUserWidget* Widget = CreateWidget<UHealthBarUserWidget>(UGameplayStatics::GetPlayerController(this, 0), HealthBarUserWidget, FName("HealthBarWidget"));
	HealthBarWidgetComponent->SetWidget(Widget);
	if (UHealthBarUserWidget* Health = Cast<UHealthBarUserWidget>(Widget))
	{
		Health->InitializeWidget(StatsComponent);
	}

	HealthBarWidgetComponent->SetVisibility(false);
}
