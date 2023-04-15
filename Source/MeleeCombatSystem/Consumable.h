// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "Consumable.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AConsumable : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	virtual void PerformItemAction() override;

	UFUNCTION(BlueprintCallable)
	void ConsumeItem();

	UFUNCTION(BlueprintCallable)
	FORCEINLINE int GetCurrentNumberOfUses() const { return CurrentNumberOfUses; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	int MaxNumberOfUses = 3;


protected:
	virtual void BeginPlay() override;

	virtual void OnEquipped() override;

public:
	FORCEINLINE UAnimMontage* GetUseItemMontage() const { return UseItemMontage; }

private:
	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimMontage* UseItemMontage;

	int CurrentNumberOfUses = 0;

};
