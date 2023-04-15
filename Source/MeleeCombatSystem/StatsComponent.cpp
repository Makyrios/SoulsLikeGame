// Fill out your copyright notice in the Description page of Project Settings.


#include "StatsComponent.h"
#include "StateManager.h"

// Sets default values for this component's properties
UStatsComponent::UStatsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

float UStatsComponent::GetCurrentStatValue(EStats Key) const
{
	const float* Value = CurrentStats.Find(Key);
	if (Value != nullptr)
	{
		return *Value;
	}
	return -1;
}

void UStatsComponent::SetCurrentStatValue(EStats Key, float Value)
{
	float ClampValue = FMath::Clamp(Value, 0, GetMaxStatValue(Key));
	CurrentStats.Add(Key, ClampValue);
	OnCurrentStatValueUpdated.Broadcast(Key, Value);
}

float UStatsComponent::GetMaxStatValue(EStats Key) const
{
	const FBaseStat* Value = BaseStats.Find(Key);
	if (Value != nullptr)
	{
		return Value->MaxValue;
	}
	return -1;
}

float UStatsComponent::GetBaseStatValue(EStats Key) const
{
	const FBaseStat* Value = BaseStats.Find(Key);
	if (Value != nullptr)
	{
		return Value->BaseValue;
	}
	return -1;
}

float UStatsComponent::ModifyCurrentStatValue(EStats Key, float Value, bool bShouldRegen)
{
	if (Value == 0) return 0;

	float NewValue = GetCurrentStatValue(Key) + Value;

	SetCurrentStatValue(Key, NewValue);

	if (bShouldRegen)
	{
		StartRegen(Key);
	}

	return GetCurrentStatValue(Key);
}

void UStatsComponent::SetBaseStatValue(EStats Key, float Value)
{
	if (BaseStats.Find(Key) != nullptr)
	{
		BaseStats.Find(Key)->BaseValue = Value;
	}
	else
	{
		BaseStats.Add(Key, FBaseStat(Value));
	}
}

void UStatsComponent::SetMaxStatValue(EStats Key, float Value)
{
	if (BaseStats.Find(Key) != nullptr)
	{
		BaseStats.Find(Key)->MaxValue = Value;
	}
	else
	{
		BaseStats.Add(Key, FBaseStat(Value));
	}
}

void UStatsComponent::TakeDamage(float Damage)
{
	// damage multiplier = damage / (damage + armor)
	// reduced damage = damage * damage multiplier
	UE_LOG(LogTemp, Display, TEXT("Armor: %f"), GetCurrentStatValue(EStats::Armor));
	float DamageMultiplier = Damage / (Damage + GetCurrentStatValue(EStats::Armor));
	float FinalDamage = Damage * DamageMultiplier;
	float NewHealth = FMath::Clamp
	(
		ModifyCurrentStatValue(EStats::Health, -FinalDamage, false),
		0,
		GetMaxStatValue(EStats::Health)
	);
	UE_LOG(LogTemp, Display, TEXT("%f damage was applied to %s"), FinalDamage, *GetOwner()->GetActorNameOrLabel());
}

void UStatsComponent::RegenStamina()
{
	float NewValue = FMath::Clamp(GetCurrentStatValue(EStats::Stamina) + StaminaRegenRate, 0, GetMaxStatValue(EStats::Stamina));
	SetCurrentStatValue(EStats::Stamina, NewValue);
	if (NewValue >= GetMaxStatValue(EStats::Stamina))
	{
		GetWorld()->GetTimerManager().ClearTimer(RegenTimerHandle);
	}
}

void UStatsComponent::InitializeStats()
{
	TSet<EStats> Keys;
	BaseStats.GetKeys(Keys);
	for (auto& Elem : Keys)
	{
		SetCurrentStatValue(Elem, BaseStats[Elem].BaseValue);
	}
}

void UStatsComponent::StartRegen(EStats StatType)
{
	/*FTimerHandle StaminaRegenDelayHandle;
	FTimerDelegate StartRegenDelegate = FTimerDelegate::CreateUObject(this, &UStatsComponent::StartRegen, StatType);
	GetWorld()->GetTimerManager().SetTimer(StaminaRegenDelayHandle, StartRegenDelegate, 1.f, false);*/
	switch (StatType)
	{
	case EStats::None:
		break;
	case EStats::Health:
		break;
	case EStats::Stamina:
		GetWorld()->GetTimerManager().SetTimer(RegenTimerHandle, this, &UStatsComponent::RegenStamina, 0.05f, true, 1.f);
		break;
	case EStats::Armor:
		break;
	}
}


