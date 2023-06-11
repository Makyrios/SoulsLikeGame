// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "MeleeCombatSystemCharacter.h"	
#include "CombatComponent.h"
#include "CollisionComponent.h"
#include <Kismet/GameplayStatics.h>
#include "Engine/DamageEvents.h"
#include "StateManager.h"
#include "AttackDamageType.h"

ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = false;

	MainWeaponCollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Main Weapon Collision Component"));
	MainWeaponCollisionComponent->ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	MainWeaponCollisionComponent->OnHitDelegate.BindUObject(this, &ABaseWeapon::OnHitEvent);

}

void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	CombatComponent = GetOwner()->FindComponentByClass<UCombatComponent>();
	if (MainWeaponCollisionComponent != nullptr)
	{
		MainWeaponCollisionComponent->AddActorToIgnore(GetOwner());
	}

	if (WeaponMeshes.Num() > 0)
	{
		ItemStaticMesh->SetStaticMesh(WeaponMeshes[FMath::RandRange(0, WeaponMeshes.Num() - 1)]);
	}
}

void ABaseWeapon::SimulateWeaponPhysics()
{
	GetStaticMesh()->SetCollisionProfileName(TEXT("PhysicsActor"));
	GetStaticMesh()->SetSimulatePhysics(true);
}

void ABaseWeapon::ActivateCollision(ECollisionPart CollisionPart)
{
	if (MainWeaponCollisionComponent != nullptr)
	{
		MainWeaponCollisionComponent->ActivateCollision();
	}
}

void ABaseWeapon::DeactivateCollision(ECollisionPart CollisionPart)
{
	if (MainWeaponCollisionComponent != nullptr)
	{
		MainWeaponCollisionComponent->DeactivateCollision();
	}
}

void ABaseWeapon::ToggleCombat(bool bEnableCombat)
{
	if (CombatComponent != nullptr)
		CombatComponent->SetIsCombatEnabled(bEnableCombat);

	if (bEnableCombat)
	{
		AttachActor(HandSocketName);
	}
	else
	{
		AttachActor(AttachSocketName);
	}
}


float ABaseWeapon::GetActionStatCost() const
{
	if (StateManager != nullptr)
	{
		const float* Value = ActionStatCost.Find(StateManager->GetCurrentAction());
		if (Value != nullptr)
		{
			return *Value;
		}
	}
	return 0;
	
}

float ABaseWeapon::GetDamage()
{
	if (StateManager != nullptr)
	{
		const float* Value = ActionDamageMultiplier.Find(StateManager->GetCurrentAction());
		if (Value != nullptr)
		{
			return BaseDamage * (*Value);
		}
	}
	return BaseDamage;
}

TArray<UAnimMontage*> ABaseWeapon::GetActionMontages(FGameplayTag Action) const
{
	if (Action.GetTagName() == "Character.Action.Attack.LightAttack")
	{
		return LightAttackMontages;
	}
	else if (Action.GetTagName() == "Character.Action.Attack.StrongAttack")
	{
		return HeavyAttackMontages;
	}
	else if (Action.GetTagName() == "Character.Action.Attack.ChargedAttack")
	{
		return ChargedAttackMontages;
	}
	else if (Action.GetTagName() == "Character.Action.Attack.FallingAttack")
	{
		return FallingAttackMontages;
	}
	else if (Action.GetTagName() == "Character.Action.Attack.SprintAttack")
	{
		return SprintAttackMontages;
	}
	else if (Action.GetTagName() == "Character.Action.BlockReaction")
	{
		return BlockReactionMontages;
	}
	else
	{
		return TArray<UAnimMontage*>();
	}
}

void ABaseWeapon::OnHitEvent(FHitResult Hit)
{
	if (ICombat* Target = Cast<ICombat>(Hit.GetActor()))
	{
		if (!Target->CanReceiveDamage())
			return;
	}

	FVector InShotDirection = -GetOwner()->GetActorForwardVector();
	InShotDirection.Normalize();

	UGameplayStatics::ApplyDamage(Hit.GetActor(), GetDamage(), GetInstigatorController(), this, DamageTypeClass);
}

void ABaseWeapon::OnEquipped()
{
	Super::OnEquipped();

	if (CombatComponent == nullptr) return;

	MainWeaponCollisionComponent->SetCollisionMesh(GetStaticMesh());

	StateManager = GetOwner()->FindComponentByClass<UStateManager>();

	if (CombatComponent->IsCombatEnabled())
	{
		AttachActor(GetHandSocketName());
	}
	else
	{
		AttachActor(GetAttachSocketName());
	}
	CombatComponent->ChangeMainWeapon(this);
}


void ABaseWeapon::OnUnequipped()
{
	Super::OnUnequipped();

	if (CombatComponent == nullptr) return;

	if (CombatComponent->IsCombatEnabled())
	{
		DetachActor();
	}
	else
	{
		DetachActor();
	}

	CombatComponent->SetMainWeapon(nullptr);
}

