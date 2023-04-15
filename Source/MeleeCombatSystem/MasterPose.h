// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "MasterPose.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AMasterPose : public ABaseEquippable
{
	GENERATED_BODY()
	
public:
	AMasterPose();

	virtual void AttachActor(FName SocketName) override;
	virtual void DetachActor() override;

	virtual void OnEquipped() override;

	virtual void OnUnequipped() override;

private:
	UPROPERTY(EditAnywhere, Category = "Stats")
	float ArmorValue;
};
