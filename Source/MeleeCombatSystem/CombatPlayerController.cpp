// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatPlayerController.h"
#include "Blueprint/UserWidget.h"


void ACombatPlayerController::BeginPlay()
{
	MainHUDWidget = CreateWidget(this, MainHUDWidgetClass);
	MainHUDWidget->AddToViewport();
}


