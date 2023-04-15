// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Interactable.h"

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class ABaseWeapon;
class ABaseEquippable;

UCLASS()
class MELEECOMBATSYSTEM_API APickupActor : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickupActor();

	virtual void Interact(AActor* Caller) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseEquippable> Item;
};
