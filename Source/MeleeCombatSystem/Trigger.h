// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "Trigger.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ATrigger : public ATriggerBox
{
	GENERATED_BODY()
	
public:
	ATrigger();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
		void OnOverlapBegin(class AActor* OverlappedActor, class AActor* OtherActor);

	UFUNCTION(BlueprintImplementableEvent)
		void OnOverlapEnd(class AActor* OverlappedActor, class AActor* OtherActor);
};
