// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CombatPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ACombatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetPotionAmount(int Amount);

	UFUNCTION(BlueprintCallable)
	UUserWidget* GetMainHUDWidget() const { return MainHUDWidget; }

protected:
	virtual void BeginPlay() override;
	

private:
	UPROPERTY(EditAnywhere, Category = "Initialization")
	TSubclassOf<UUserWidget> MainHUDWidgetClass;


	UUserWidget* MainHUDWidget;
};
