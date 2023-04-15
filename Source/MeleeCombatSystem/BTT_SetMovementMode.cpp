// Fill out your copyright notice in the Description page of Project Settings.


#include "BTT_SetMovementMode.h"
#include "AIController.h"
#include "MasterAI.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTT_SetMovementMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr) return EBTNodeResult::Failed;

	AMasterAI* MasterAI = Cast<AMasterAI>(Pawn);
	if (MasterAI == nullptr) return EBTNodeResult::Failed;

	MasterAI->SetMovementSpeedMode(SpeedMode);

	return EBTNodeResult::Succeeded;
}
