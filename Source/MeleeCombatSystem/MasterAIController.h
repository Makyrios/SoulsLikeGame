// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MasterAIController.generated.h"

class AMasterAI;

UCLASS()
class MELEECOMBATSYSTEM_API AMasterAIController : public AAIController
{
	GENERATED_BODY()

	AMasterAIController();
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdatePerception(const TArray<AActor*>& PerceivedActors);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Initialization")
	UAIPerceptionComponent* AIPerceptionComponent;

	virtual void OnPossess(APawn* InPawn) override;

private:
	AMasterAI* MasterPawn;


	void SetBossHealthVisibility(bool bIsVisible);

	UFUNCTION()
	void OnPerceptionUpdate(const TArray<AActor*>& UpdatedActors);

};
