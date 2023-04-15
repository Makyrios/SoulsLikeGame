// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManager.h"

// Sets default values
AStateManager::AStateManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

bool AStateManager::IsCurrentStateEqualToAny(TArray<ECharacterState>& CharacterStates)
{
	return CharacterStates.Contains(CurrentCharacterState);
}

// Called when the game starts or when spawned
void AStateManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStateManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStateManager::SetCurrentState(ECharacterState NewState)
{
	if (CurrentCharacterState != NewState)
	{
		OnStateEndEvent.Broadcast(CurrentCharacterState);
		CurrentCharacterState = NewState;
		OnStateBeginEvent.Broadcast(NewState);
	}
}

