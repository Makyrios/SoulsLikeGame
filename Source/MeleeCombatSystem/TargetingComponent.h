// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ERotationMode.h"
#include "TargetingComponent.generated.h"

class ACharacter;
class AController;
class UCameraComponent;
class USpringArmComponent;
class UCombatComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ToggleLockOn();


public:
	FORCEINLINE bool IsTargeting() const { return bIsTargeting; }
	void SetIsTargeting(bool Value);

	FORCEINLINE AActor* GetTargetActor() const { return TargetActor; }
	FORCEINLINE void SetTargetActor(AActor* NewActor) { TargetActor = NewActor; }

	FORCEINLINE ERotationMode GetRotationMode() const { return CurrentRotationMode; }
	void SetRotationMode(ERotationMode Mode);

	//void UpdateRotationMode();

private:
	UPROPERTY(EditAnywhere, Category = "Initialization")
	float TargetingDistance = 500;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	float TargetingRadius = 100;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	float CameraLockInterpolationSpeed = 500;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<AActor*> ActorsToIgnore;

	ACharacter* OwnerCharacter;
	AController* OwnerController;
	UCameraComponent* FollowCamera;
	USpringArmComponent* CameraBoom;
	UCombatComponent* CombatComponent;

	AActor* TargetActor = nullptr;

	ERotationMode CurrentRotationMode;

	bool bIsTargeting;


	void EnableLockOn();
	void DisableLockOn();
	bool FindTarget(AActor** FoundTarget);

	void UpdateTargetingControlRotation(float DeltaTime);

	bool CanTargetActor(AActor* Actor) const;
};
