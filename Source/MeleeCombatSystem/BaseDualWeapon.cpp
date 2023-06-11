// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseDualWeapon.h"
#include "CollisionComponent.h"
#include "GameFramework/Character.h"
#include "CombatComponent.h"

ABaseDualWeapon::ABaseDualWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	SecondWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seconds Weapon Mesh"));
	SecondWeaponMesh->SetupAttachment(RootComponent);

	SecondWeaponCollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Second Weapon Collision Component"));
}

void ABaseDualWeapon::OnEquipped()
{
	SecondWeaponCollisionComponent->SetCollisionMesh(SecondWeaponMesh);

	// Attach main weapon
	Super::OnEquipped();

}

void ABaseDualWeapon::OnUnequipped()
{
	Super::OnUnequipped();

	if (CombatComponent->IsCombatEnabled())
	{
		DetachActor();
	}
	else
	{
		DetachActor();
	}
}

void ABaseDualWeapon::ActivateCollision(ECollisionPart CollisionPart)
{
	switch (CollisionPart)
	{
	case ECollisionPart::MainWeapon:
		if (MainWeaponCollisionComponent != nullptr)
			MainWeaponCollisionComponent->ActivateCollision();
		break;
	case ECollisionPart::OffHandWeapon:
		if (SecondWeaponCollisionComponent != nullptr)
		SecondWeaponCollisionComponent->ActivateCollision();
	}

}

void ABaseDualWeapon::DeactivateCollision(ECollisionPart CollisionPart)
{
	Super::DeactivateCollision(CollisionPart);

	switch (CollisionPart)
	{
	case ECollisionPart::MainWeapon:
		if (MainWeaponCollisionComponent != nullptr)
			MainWeaponCollisionComponent->DeactivateCollision();
		break;
	case ECollisionPart::OffHandWeapon:
		if (SecondWeaponCollisionComponent != nullptr)
			SecondWeaponCollisionComponent->DeactivateCollision();
	}
}

void ABaseDualWeapon::AttachActor(FName SocketName)
{
	// Attach main(right hand) weapon
	Super::AttachActor(SocketName);


	if (CombatComponent->IsCombatEnabled())
	{
		AttachOffhandActor(SecondWeaponHandSocket);
	}
	else
	{
		AttachOffhandActor(SecondWeaponAttachSocket);
	}
}

void ABaseDualWeapon::AttachOffhandActor(FName SocketName)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr && SecondWeaponMesh != nullptr)
	{
		SecondWeaponMesh->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void ABaseDualWeapon::DetachOffhandActor(FName SocketName)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr && SecondWeaponMesh != nullptr)
	{
		SecondWeaponMesh->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void ABaseDualWeapon::ToggleCombat(bool bEnableCombat)
{
	Super::ToggleCombat(bEnableCombat);

	if (bEnableCombat)
	{
		AttachOffhandActor(SecondWeaponHandSocket);
	}
	else
	{
		AttachOffhandActor(SecondWeaponAttachSocket);
	}
}


void ABaseDualWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (SecondWeaponCollisionComponent != nullptr)
	{
		SecondWeaponCollisionComponent->OnHitDelegate.BindUObject(this, &ABaseDualWeapon::OnHitEvent);
		SecondWeaponCollisionComponent->AddActorToIgnore(GetOwner());
	}
}

