// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "CollisionComponent.generated.h"

DECLARE_DELEGATE_OneParam(FOnHitDelegate, FHitResult)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UCollisionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCollisionComponent();

	FOnHitDelegate OnHitDelegate;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	bool bShouldUseWeaponMesh = true;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetCollisionMesh(UPrimitiveComponent* Value);

	void ActivateCollision();
	FORCEINLINE void DeactivateCollision() { bIsCollisionEnabled = false; }

	FORCEINLINE void AddActorToIgnore(AActor* Actor) { ActorsToIgnore.Add(Actor); }

private:
	void CollisionTrace();

	FORCEINLINE void ClearHitActors() { AlreadyHitActors.Empty(); }

private:
	bool bIsCollisionEnabled;

	TArray<AActor*> AlreadyHitActors;

	UPrimitiveComponent* CollisionMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	FName StartSocketName = "WeaponStart";
	UPROPERTY(EditAnywhere, Category = "Initialization")
	FName EndSocketName = "WeaponEnd";

	UPROPERTY(EditAnywhere, Category = "Initialization")
	float TraceRadius = 20;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<AActor*> ActorsToIgnore;

};
