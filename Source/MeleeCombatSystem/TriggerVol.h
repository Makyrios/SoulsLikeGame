// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "TriggerVol.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ATriggerVol : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ATriggerVol();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
	void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
	
};
