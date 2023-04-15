// Fill out your copyright notice in the Description page of Project Settings.


#include "StateManager.h"

// Sets default values for this component's properties
UStateManager::UStateManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStateManager::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UStateManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	/*if (GetOwner()->ActorHasTag(FName("Dummy")))
	{
		UE_LOG(LogTemp, Warning, TEXT("Current State %s"), *GetCurrentState().ToString());
	}*/
	// ...
}

void UStateManager::ResetState()
{
	SetCurrentState(FGameplayTag::EmptyTag);
}

void UStateManager::ResetAction()
{
	SetCurrentAction(FGameplayTag::EmptyTag);
}

bool UStateManager::IsCurrentStateEqualToAny(FGameplayTagContainer& CharacterStates) const
{
	//UE_LOG(LogTemp, Warning, TEXT("Current State %s"), *GetCurrentState().ToString());
	return CharacterStates.HasTagExact(CurrentCharacterState);
	//return CharacterStates.Contains(CurrentCharacterState);
}

bool UStateManager::IsCurrentActionEqualToAny(FGameplayTagContainer& CharacterActions) const
{
	return CharacterActions.HasTagExact(CurrentCharacterAction);
	//return CharacterActions.Contains(CurrentCharacterAction);
}



void UStateManager::SetCurrentState(FGameplayTag NewState)
{
	//if (CurrentCharacterState == FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")))
	//{
	//	UE_LOG(LogTemp, Display, TEXT("Do not reset state if dead"));
	//	return;
	//}
	if (CurrentCharacterState != NewState)
	{
		if (OnStateEndEvent.IsBound())
			OnStateEndEvent.Broadcast(CurrentCharacterState);
		CurrentCharacterState = NewState;
		if (OnStateBeginEvent.IsBound())
			OnStateBeginEvent.Broadcast(NewState);
	}
}

void UStateManager::SetCurrentAction(FGameplayTag NewAction)
{
	if (CurrentCharacterAction != NewAction)
	{
		if (OnActionEndEvent.IsBound())
			OnActionEndEvent.Broadcast(CurrentCharacterAction);
		CurrentCharacterAction = NewAction;
		if (OnStateBeginEvent.IsBound())
			OnActionBeginEvent.Broadcast(NewAction);
	}
}
