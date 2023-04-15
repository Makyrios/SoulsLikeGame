// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseDualWeapon.h"
#include "FistsWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API AFistsWeapon : public ABaseDualWeapon
{
	GENERATED_BODY()
	
public:
	virtual void OnEquipped() override;
};
