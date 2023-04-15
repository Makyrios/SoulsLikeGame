// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EStats.h"
#include "StatsComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCurrentStatValueUpdated, EStats, StatType, float, Value);

USTRUCT(BlueprintType)
struct FBaseStat
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float BaseValue;
	UPROPERTY(EditAnywhere)
	float MaxValue;

	FBaseStat()
	{
		MaxValue = 100;
		BaseValue = MaxValue;
	}

	FBaseStat(float Value)
	{
		MaxValue = Value;
		BaseValue = MaxValue;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatsComponent();

	UPROPERTY(BlueprintAssignable)
	FOnCurrentStatValueUpdated OnCurrentStatValueUpdated;

	UPROPERTY(EditAnywhere, Category = "Base Stats")
	TMap<EStats, FBaseStat> BaseStats;


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void InitializeStats();

	UFUNCTION(BlueprintCallable)
	float GetCurrentStatValue(EStats Key) const;

	UFUNCTION(BlueprintCallable)
	float GetMaxStatValue(EStats Key) const;

	float GetBaseStatValue(EStats Key) const;

	UFUNCTION(BlueprintCallable)
	float ModifyCurrentStatValue(EStats Key, float Value, bool bShouldRegen = true);

	void SetBaseStatValue(EStats Key, float Value);

	void SetMaxStatValue(EStats Key, float Value);

	void TakeDamage(float Damage);

	void StartRegen(EStats StatType);


private:
	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRegenRate;

	TMap<EStats, float> CurrentStats;

private:
	FTimerHandle RegenTimerHandle;


	void RegenStamina();

	void SetCurrentStatValue(EStats Key, float Value);
};
