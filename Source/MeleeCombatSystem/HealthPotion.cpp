// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthPotion.h"
#include "Combat.h"

void AHealthPotion::BeginPlay()
{
	Super::BeginPlay();

	if (ICombat* Player = Cast<ICombat>(GetOwner()))
	{
		Player->UpdateHealthPotionAmount(MaxNumberOfUses);
	}
}
