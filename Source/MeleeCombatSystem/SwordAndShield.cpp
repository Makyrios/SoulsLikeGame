// Fill out your copyright notice in the Description page of Project Settings.


#include "SwordAndShield.h"
#include "CombatComponent.h"
#include "CollisionComponent.h"
#include "GameFramework/Character.h"

ASwordAndShield::ASwordAndShield()
{
	PrimaryActorTick.bCanEverTick = true;

	ShieldStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Shield Static Mesh"));
	ShieldStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ASwordAndShield::OnEquipped()
{
	Super::OnEquipped();

	if (CombatComponent->IsCombatEnabled())
	{
		AttachShield(ShieldHandSocket);
	}
	else
	{
		AttachShield(ShieldAttachSocket);
	}

	CombatComponent->SetShieldEquipped(true);
}

void ASwordAndShield::OnUnequipped()
{
	Super::OnUnequipped();

	if (CombatComponent->IsCombatEnabled())
	{
		DetachShield();
	}
	else
	{
		DetachShield();
	}

	CombatComponent->SetShieldEquipped(false);
}


void ASwordAndShield::ToggleCombat(bool bEnableCombat)
{
	Super::ToggleCombat(bEnableCombat);

	if (bEnableCombat)
	{
		AttachShield(ShieldHandSocket);
	}
	else
	{
		AttachShield(ShieldAttachSocket);
	}
}

void ASwordAndShield::AttachShield(FName AttachSocket)
{
	ShieldStaticMesh->AttachToComponent(Cast<ACharacter>(GetOwner())->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachSocket);
}

void ASwordAndShield::DetachShield()
{
	ShieldStaticMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
}
