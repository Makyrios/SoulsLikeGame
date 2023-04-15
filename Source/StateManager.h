// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ECharacterState.h"
#include "StateManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateBegin, ECharacterState, CharacterState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStateEnd, ECharacterState, CharacterState);

UCLASS()
class MELEECOMBATSYSTEM_API AStateManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStateManager();

	FOnStateBegin OnStateBeginEvent;
	FOnStateEnd OnStateEndEvent;

	FORCEINLINE void ResetState() { CurrentCharacterState = ECharacterState::Nothing; }

	bool IsCurrentStateEqualToAny(TArray<ECharacterState>& CharacterStates);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE ECharacterState GetCurrentState() const { return CurrentCharacterState; }
	void SetCurrentState(ECharacterState NewState);

private:
	ECharacterState CurrentCharacterState;
};
