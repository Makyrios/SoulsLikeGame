// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"

class ABaseEquippable;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEquipmentComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeEquipment();

	UFUNCTION(BlueprintCallable)
	bool UseItemByTag(FGameplayTag ItemTag);

	UFUNCTION(BlueprintCallable)
	ABaseEquippable* GetItemByTag(FGameplayTag ItemTag);

	void EquipItem(TSubclassOf<ABaseEquippable> Item);

	void UnequipItem(ABaseEquippable* ItemToUnequip);


	/// <summary>
	/// Check if two objects have same common base custom(not including UE classes) class
	/// </summary>
	/// <param name="Object1">First object to check</param>
	/// <param name="Object2">Second object to check</param>
	/// <returns>True if two objects have same base class, false if otherwise</returns>
	static bool CheckIfRelatedByParent(UClass* Object1, UClass* Object2);

public:
	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<TSubclassOf<ABaseEquippable>> StartingEquipment;

private:
	TArray<ABaseEquippable*> EquippedItems;

};
