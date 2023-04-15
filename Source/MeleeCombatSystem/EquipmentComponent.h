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

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<TSubclassOf<ABaseEquippable>> StartingEquipment;


private:
	TArray<ABaseEquippable*> EquippedItems;


	bool CheckIfRelatedByParent(UClass* Object1, UClass* Object2) const;
};
