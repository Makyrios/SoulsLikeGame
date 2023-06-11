// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "BaseWeapon.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MeleeCombatSystemCharacter.h"
#include "StatsComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	OwnerStateControl = Cast<IStateControl>(OwnerCharacter);
	OwnerStatsComponent = GetOwner()->FindComponentByClass<UStatsComponent>();
}

/** Play draw/disarm animation */
void UCombatComponent::ToggleCombat()
{
	if (OwnerStateControl == nullptr)
	{
		return;
	}

	// Disarm animation
	if (bIsCombatEnabled)
	{
		OwnerStateControl->PerformAction(FGameplayTag::RequestGameplayTag(FName("Character.Action.ExitCombat")),
			FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));
	}
	// Draw animation
	else
	{
		OwnerStateControl->PerformAction(FGameplayTag::RequestGameplayTag(FName("Character.Action.EnterCombat")),
			FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));
	}
}

/** Play animation of specified attack by index or choose random attack from array */
float UCombatComponent::PerformAttack(FGameplayTag AttackType, int AttackIndex, bool bRandomIndex = false)
{
	if (MainWeapon == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No MainWeapon"));
		return -1;
	}
	if (OwnerStateControl == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No StateControl"));
		return -1;
	}

	// Save attack
	// If clicking attack button while Attacking state, then save this attack and it's type
	if (OwnerStateControl->GetCurrentState() == FGameplayTag::RequestGameplayTag(FName("Character.State.Attacking")))
	{
		UE_LOG(LogTemp, Display, TEXT("Save attack"));
		bIsAttackSaved = true;
		SavedAttack = AttackType;
		return 0;
	}
	
	TArray<UAnimMontage*> AttackMontages = OwnerStateControl->GetMontageByTag(AttackType);
	int FinalIndex = 0;
	// Get either random animation of the animations array, or attack at specified index, or first attack of the array
	if (AttackMontages.Num() > 0)
	{
		if (bRandomIndex)
		{
			FinalIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
		}
		else
		{
			if (AttackMontages.IsValidIndex(AttackIndex))
			{
				FinalIndex = AttackIndex;
			}
			else
			{
				FinalIndex = 0;
			}
		}
	}

	AttackCount += 1;
	// Used to make a loop of attacks of the same type
	if (AttackCount >= AttackMontages.Num())
	{
		AttackCount = 0;
	}

	UE_LOG(LogTemp, Warning, TEXT("Perform attack"));
	return OwnerStateControl->PerformAction(AttackType,
		FGameplayTag::RequestGameplayTag(FName("Character.State.Attacking")), false, FinalIndex);
}

void UCombatComponent::PerformNextAttack(FGameplayTag AttackType)
{
	if (OwnerStateControl != nullptr)
	{
		OwnerStateControl->ResetState();
	}

	if (bIsAttackSaved)
	{
		bIsAttackSaved = false;
		PerformAttack(AttackType, AttackCount);
	}
}

void UCombatComponent::ResetAttack()
{
	AttackCount = 0;
	bIsAttackSaved = false;
}

void UCombatComponent::ResetCombat()
{
	ResetAttack();
	if (OwnerStateControl != nullptr)
	{
		OwnerStateControl->ResetState();
		OwnerStateControl->ResetAction();
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::ChangeMainWeapon(ABaseWeapon* Value)
{
	if (MainWeapon != nullptr)
	{
		MainWeapon->Destroy();
		MainWeapon->OnUnequipped();
	}
	MainWeapon = Value;
	OnWeaponEquipped.Broadcast(Value->GetCombatType());
}


void UCombatComponent::SetIsCombatEnabled(bool Value)
{
	bIsCombatEnabled = Value;
	OnCombatEnabled.Broadcast(Value);
}

void UCombatComponent::SetBlockingState(bool bEnableBlock)
{
	if (bIsBlocking != bEnableBlock)
	{
		bIsBlocking = bEnableBlock;
		OnBlockingSet.Broadcast(bEnableBlock);
	}
}

void UCombatComponent::SetShieldEquipped(bool Value)
{
	bIsShieldEquipped = Value;

	OnShieldEquippedSet.Broadcast(Value);
}


