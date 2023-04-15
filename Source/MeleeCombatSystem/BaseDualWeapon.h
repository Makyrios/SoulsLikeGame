// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "BaseDualWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ABaseDualWeapon : public ABaseWeapon
{
	GENERATED_BODY()

public:
	ABaseDualWeapon();

	virtual void OnEquipped() override;

	virtual void OnUnequipped() override;

	virtual void ActivateCollision(ECollisionPart CollisionPart) override;
	virtual void DeactivateCollision(ECollisionPart CollisionPart) override;

	virtual void AttachActor(FName SocketName) override;

	void AttachOffhandActor(FName SocketName);
	void DetachOffhandActor(FName SocketName);

	virtual void ToggleCombat(bool bEnableCombat) override;


protected:
	virtual void BeginPlay() override;


private:
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	UStaticMeshComponent* SecondWeaponMesh;
	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	class UCollisionComponent* SecondWeaponCollisionComponent;

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName SecondWeaponAttachSocket;
	UPROPERTY(EditAnywhere, Category = "Socket")
	FName SecondWeaponHandSocket;

	
};
