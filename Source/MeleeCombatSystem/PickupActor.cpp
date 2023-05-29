// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupActor.h"
#include "BaseWeapon.h"
#include "BaseEquippable.h"
#include "MeleeCombatSystemCharacter.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"

// Sets default values
APickupActor::APickupActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

void APickupActor::Interact(AActor* Caller)
{
	UE_LOG(LogTemp, Display, TEXT("Pick up: %s"), *GetName());

	UEquipmentComponent* EquipComponent = Caller->FindComponentByClass<UEquipmentComponent>();
	UInventoryComponent* InventoryComponent = Caller->FindComponentByClass<UInventoryComponent>();

	EquipComponent->EquipItem(Item);
	if (TSubclassOf<ABaseWeapon> Weapon = Item)
	{
		UE_LOG(LogTemp, Display, TEXT("Add weapon to inventory: %s"), *Weapon->GetName());
		InventoryComponent->AddWeapon(Weapon);
	}
}

// Called when the game starts or when spawned
void APickupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APickupActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

