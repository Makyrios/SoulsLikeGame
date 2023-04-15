// Fill out your copyright notice in the Description page of Project Settings.


#include "TestDummyCharacter.h"
#include "MasterPose.h"
#include "StateManager.h"
#include "Components/WidgetComponent.h"

ATestDummyCharacter::ATestDummyCharacter()
{
	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOn Widget Component"));
	LockOnWidgetComponent->SetupAttachment(RootComponent);
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetDrawSize(FVector2D(14.0f, 14.0f));
}

void ATestDummyCharacter::BeginPlay()
{
	Super::BeginPlay();

	LockOnWidgetComponent->SetVisibility(false);

}

bool ATestDummyCharacter::CanBeTargeted()
{
	if (StateManager != nullptr)
	{
		FGameplayTagContainer IsDead(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
		return !StateManager->IsCurrentStateEqualToAny(IsDead);
	}
	return false;
}

void ATestDummyCharacter::OnTargeted(bool bIsTargeted)
{
	if (LockOnWidgetComponent == nullptr) return;

	if (bIsTargeted)
	{
		LockOnWidgetComponent->SetVisibility(true);
	}
	else
	{
		LockOnWidgetComponent->SetVisibility(false);
	}
}

