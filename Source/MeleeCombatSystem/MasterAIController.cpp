// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterAIController.h"
#include "MasterAI.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AIPerceptionTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BossEnemy.h"

AMasterAIController::AMasterAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception");
	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AMasterAIController::OnPerceptionUpdate);

}

void AMasterAIController::UpdatePerception(const TArray<AActor*>& PerceivedActors)
{
	UAIPerceptionComponent* AIPerception = FindComponentByClass<UAIPerceptionComponent>();
	UBlackboardComponent* BlackBoard = GetBlackboardComponent();
	FName TargetObjectName = TEXT("Target");
	for (auto Actor : PerceivedActors)
	{
		if (AIPerception != nullptr)
		{
			FActorPerceptionBlueprintInfo Info;
			if (AIPerception->GetActorsPerception(Actor, Info) == false)
			{
				return;
			}

			for (int i = 0; i < Info.LastSensedStimuli.Num(); i++)
			{
				// Sight
				if (Info.LastSensedStimuli[i].Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
				{
					if (Info.LastSensedStimuli[i].WasSuccessfullySensed())
					{
						if (!Actor->ActorHasTag("Character.Enemy"))
						{
							//UE_LOG(LogTemp, Warning, TEXT("Set Target Actor: %s"), *Actor->GetActorLabel());
							BlackBoard->SetValueAsObject(TargetObjectName, Actor);
							SetBossHealthVisibility(true);
							MasterPawn->OnTargetSet(Actor);
						}
					}
					else
					{
						BlackBoard->ClearValue(TargetObjectName);
						//UE_LOG(LogTemp, Warning, TEXT("Clear Target Actor: %s"), *Actor->GetActorLabel());
						SetBossHealthVisibility(false);
						MasterPawn->OnTargetSet(nullptr);
					}
				}
				// Damage
				else if (Info.LastSensedStimuli[i].Type == UAISense::GetSenseID(UAISense_Damage::StaticClass()))
				{
					if (Info.LastSensedStimuli[i].WasSuccessfullySensed())
					{
						//UE_LOG(LogTemp, Warning, TEXT("Damage Set value of %s"), *Actor->GetActorLabel());
						BlackBoard->SetValueAsObject(TargetObjectName, Actor);
						SetBossHealthVisibility(true);
						MasterPawn->OnTargetSet(Actor);
					}
				}
			}
		}

	}
	if (Cast<IStateControl>(GetPawn())->GetCurrentState() == FGameplayTag::RequestGameplayTag("Character.State.Dead"))
	{
		AIPerceptionComponent->OnPerceptionUpdated.RemoveDynamic(this, &AMasterAIController::OnPerceptionUpdate);
	}
}

void AMasterAIController::SetBossHealthVisibility(bool bIsVisible)
{
	if (ABossEnemy* BossPawn = Cast<ABossEnemy>(MasterPawn))
	{
		BossPawn->SetBossHealthVisibility(bIsVisible);
	}
}

void AMasterAIController::OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors)
{
	UpdatePerception(UpdatedActors);
}

void AMasterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MasterPawn = Cast<AMasterAI>(InPawn);

	if (MasterPawn == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is attached to not MasterAI derived class"));
	}
	RunBehaviorTree(MasterPawn->GetBehaviorTree());
}

