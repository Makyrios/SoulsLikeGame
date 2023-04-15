// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseShield.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ABaseShield : public ABaseWeapon
{
	GENERATED_BODY()
	
public:
	ABaseShield();

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	virtual TArray<UAnimMontage*> GetActionMontages(FGameplayTag Action) const override;


	FORCEINLINE float GetDamageReductionMultiplier() const { return DamageReductionMultiplier; }

private:
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> BlockAnimations;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float DamageReductionMultiplier;
};
