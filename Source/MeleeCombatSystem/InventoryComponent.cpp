// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "MeleeCombatSystemCharacter.h"
#include "CombatComponent.h"
#include "BaseWeapon.h"
#include "EquipmentComponent.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::AddWeapon(TSubclassOf<ABaseWeapon> Weapon)
{
	EquippedWeapons.AddUnique(Weapon);
}

void UInventoryComponent::ChangeWeapon()
{
	AMeleeCombatSystemCharacter* Owner = Cast<AMeleeCombatSystemCharacter>(GetOwner());
	if (Owner == nullptr)
		return;

	CurrentWeaponSlot += 1;

	if (CurrentWeaponSlot > EquippedWeapons.Num() - 1)
	{
		CurrentWeaponSlot = 0;
	}

	//TSubclassOf<ABaseWeapon> NewWeapon = EquippedWeapons.IsValidIndex(CurrentWeaponSlot) ? EquippedWeapons[CurrentWeaponSlot] : EquippedWeapons[0];
	TSubclassOf<ABaseWeapon> NewWeapon = EquippedWeapons[CurrentWeaponSlot];

	//if (NewWeapon != TSubclassOf<ABaseWeapon>(Owner->GetCombatComponent()->GetMainWeapon()->GetClass()))
	//{
		Owner->GetEquipmentComponent()->EquipItem(NewWeapon);
	//}
}

void UInventoryComponent::InitializeWeapons()
{
	AMeleeCombatSystemCharacter* Owner = Cast<AMeleeCombatSystemCharacter>(GetOwner());
	if (Owner == nullptr)
		return;

	if (Owner->GetCombatComponent()->GetMainWeapon() != nullptr)
	{
		EquippedWeapons.AddUnique(TSubclassOf<ABaseWeapon>(Owner->GetCombatComponent()->GetMainWeapon()->GetClass()));
	}
}

