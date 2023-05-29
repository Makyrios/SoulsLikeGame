// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MasterAI.h"
#include "HealthBarUserWidget.h"
#include "RegularEnemy.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ARegularEnemy : public AMasterAI
{
	GENERATED_BODY()

public:
	ARegularEnemy();

	virtual void OnTargeted(bool bIsTargeted) override;

protected:
	void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "HUD", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* HealthBarWidgetComponent;

	UPROPERTY(EditAnywhere, Category = "HUD")
	TSubclassOf<UHealthBarUserWidget> HealthBarUserWidget;
	
};
