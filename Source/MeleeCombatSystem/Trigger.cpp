// Fill out your copyright notice in the Description page of Project Settings.


#include "Trigger.h"

ATrigger::ATrigger()
{
	OnActorBeginOverlap.AddDynamic(this, &ATrigger::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATrigger::OnOverlapEnd);
}

void ATrigger::BeginPlay()
{
	Super::BeginPlay();

}