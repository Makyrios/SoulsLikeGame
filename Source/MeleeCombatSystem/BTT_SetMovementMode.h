// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "EMovementSpeedMode.h"
#include "BTT_SetMovementMode.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API UBTT_SetMovementMode : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Initialization")
	EMovementSpeedMode SpeedMode;
	
};
