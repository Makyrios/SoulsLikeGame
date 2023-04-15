// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDualWeapon.h"
#include "ToughDualSwords.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AToughDualSwords : public ABaseDualWeapon
{
	GENERATED_BODY()
	
public:
	AToughDualSwords();

	virtual void OnEquipped() override;

	virtual void ActivateCollision(ECollisionPart CollisionPart) override;
	virtual void DeactivateCollision(ECollisionPart CollisionPart) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	class UCollisionComponent* RightFootCollisionComponent;
};
