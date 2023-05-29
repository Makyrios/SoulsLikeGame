// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "BaseEquippable.h"
#include "BaseWeapon.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UEquipmentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UEquipmentComponent::UseItemByTag(FGameplayTag ItemTag)
{
	for (ABaseEquippable* Item : EquippedItems)
	{
		if (Item->HasMatchingGameplayTag(ItemTag))
		{
			Item->PerformItemAction();
			return true;
		}
	}
	return false;
}

ABaseEquippable* UEquipmentComponent::GetItemByTag(FGameplayTag ItemTag)
{

	for (ABaseEquippable* Item : EquippedItems)
	{
		if (Item->HasMatchingGameplayTag(ItemTag))
		{
			return Item;
		}
	}
	return nullptr;

}

void UEquipmentComponent::EquipItem(TSubclassOf<ABaseEquippable> Item)
{
	UClass* ItemClass = Item.Get();

	for (auto EquippedItem : EquippedItems)
	{
		// If two objects have same base custom class (i.e. BaseWeapon, MasterAI) then unequip this item
		if (CheckIfRelatedByParent(EquippedItem->GetClass(), ItemClass))
		{
			UnequipItem(EquippedItem);
			break;
		}
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = GetOwner();
	SpawnParameters.Instigator = Cast<APawn>(GetOwner());
	ABaseEquippable* NewItem = GetWorld()->SpawnActor<ABaseEquippable>(Item, GetOwner()->GetActorTransform(), SpawnParameters);
	if (NewItem != nullptr)
	{
		NewItem->OnEquipped();
		EquippedItems.AddUnique(NewItem);
	}
}

void UEquipmentComponent::UnequipItem(ABaseEquippable* ItemToUnequip)
{
	if (ItemToUnequip != nullptr)
	{
		EquippedItems.Remove(ItemToUnequip);
		ItemToUnequip->OnUnequipped();
		ItemToUnequip->Destroy();
	}
}

bool UEquipmentComponent::CheckIfRelatedByParent(UClass* Object1, UClass* Object2)
{
	if (!Object1 || !Object2)
	{
		// One of the actors is null, they are not related by parent
		return false;
	}

	UClass* Parent1 = Object1->GetSuperClass();
	UClass* Parent2 = Object2->GetSuperClass();

	while (Parent1 && Parent1 != ABaseEquippable::StaticClass())
	{
		if (Object2->IsChildOf(Parent1))
		{
			// Object2 is a parent or ancestor of Object1
			return true;
		}
		Parent1 = Parent1->GetSuperClass();
	}

	while (Parent2 && Parent2 != ABaseEquippable::StaticClass())
	{
		if (Object1->IsChildOf(Parent2))
		{
			// Object1 is a parent or ancestor of Object2
			return true;
		}
		Parent2 = Parent2->GetSuperClass();
	}

	// Actors are not related by parent
	return false;
}

void UEquipmentComponent::InitializeEquipment()
{
	if (StartingEquipment.Num() > 0)
	{
		FVector ActorLocation = GetOwner()->GetActorLocation();
		FRotator ActorRotation = GetOwner()->GetActorRotation();

		// Equip items
		for (auto Item : StartingEquipment)
		{
			FActorSpawnParameters Parameters;
			Parameters.Instigator = GetOwner()->GetInstigator();
			Parameters.Owner = GetOwner();
			ABaseEquippable* SpawnActor = GetWorld()->SpawnActor<ABaseEquippable>(Item, ActorLocation, ActorRotation, Parameters);
			if (SpawnActor != nullptr)
			{
				EquippedItems.AddUnique(SpawnActor);
				SpawnActor->OnEquipped();
			}
		}
	}
}

