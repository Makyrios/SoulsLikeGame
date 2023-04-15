// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseShield.h"
#include "CombatComponent.h"
#include "CollisionComponent.h"

ABaseShield::ABaseShield()
{
}

void ABaseShield::OnEquipped()
{
	bIsEquipped = true;

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
	//CombatComponent->SetShieldWeapon(this);
}

void ABaseShield::OnUnequipped()
{
	bIsEquipped = false;

	if (CombatComponent == nullptr) return;

	if (CombatComponent->IsCombatEnabled())
	{
		DetachActor();
	}
	else
	{
		DetachActor();
	}

	//CombatComponent->SetShieldWeapon(nullptr);
}

TArray<UAnimMontage*> ABaseShield::GetActionMontages(FGameplayTag Action) const
{
	Super::GetActionMontages(Action);

	if (Action.GetTagName() == "Character.Action.BlockReaction")
	{
		return BlockAnimations;
	}

	return TArray<UAnimMontage*>();
}



