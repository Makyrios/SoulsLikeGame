// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SwordAndShield.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ASwordAndShield : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ASwordAndShield();

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	virtual void ToggleCombat(bool bEnableCombat) override;

	FORCEINLINE float GetDamageReductionMultiplier() const { return DamageReductionMultiplier; }


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* ShieldStaticMesh;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
		float DamageReductionMultiplier;

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
		FName ShieldAttachSocket = "ShieldBack";

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
		FName ShieldHandSocket = "ShieldHand";


	void AttachShield(FName AttachSocket);
	void DetachShield();
};
