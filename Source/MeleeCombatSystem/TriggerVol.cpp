// Fill out your copyright notice in the Description page of Project Settings.


#include "TriggerVol.h"

ATriggerVol::ATriggerVol()
{
	OnActorBeginOverlap.AddDynamic(this, &ATriggerVol::OnOverlapBegin);
	OnActorEndOverlap.AddDynamic(this, &ATriggerVol::OnOverlapEnd);
}

void ATriggerVol::BeginPlay()
{
	Super::BeginPlay();

}


