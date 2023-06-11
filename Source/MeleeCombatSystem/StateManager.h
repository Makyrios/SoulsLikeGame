// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECharacterState.h"
#include "ECharacterAction.h"
#include "GameplayTags.h"
#include "StateManager.generated.h"

DECLARE_EVENT_OneParam(UStateManager, FOnStateBegin, FGameplayTag);
DECLARE_EVENT_OneParam(UStateManager, FOnStateEnd, FGameplayTag);

DECLARE_EVENT_OneParam(UStateManager, FOnActionBegin, FGameplayTag);
DECLARE_EVENT_OneParam(UStateManager, FOnActionEnd, FGameplayTag);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UStateManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateManager();

	FOnStateBegin OnStateBeginEvent;
	FOnStateEnd OnStateEndEvent;

	FOnActionBegin OnActionBeginEvent;
	FOnActionEnd OnActionEndEvent;

	void ResetState();
	void ResetAction();

	bool IsCurrentStateEqualToAny(FGameplayTagContainer& CharacterStates) const;

	bool IsCurrentActionEqualToAny(FGameplayTagContainer& CharacterActions) const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FGameplayTag GetCurrentState() const { return CurrentCharacterState; }
	void SetCurrentState(FGameplayTag NewState);

	FORCEINLINE FGameplayTag GetCurrentAction() const { return CurrentCharacterAction; }
	void SetCurrentAction(FGameplayTag NewAction);

private:
	FGameplayTag CurrentCharacterState;

	FGameplayTag CurrentCharacterAction;
};
