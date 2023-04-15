// Fill out your copyright notice in the Description page of Project Settings.


#include "ToughDualSwords.h"
#include "CollisionComponent.h"
#include "GameFramework/Character.h"

AToughDualSwords::AToughDualSwords()
{
	RightFootCollisionComponent = CreateDefaultSubobject<UCollisionComponent>(TEXT("Right Foot Collision Component"));
}

void AToughDualSwords::OnEquipped()
{
	RightFootCollisionComponent->SetCollisionMesh(Cast<ACharacter>(GetOwner())->GetMesh());
	Super::OnEquipped();
}

void AToughDualSwords::ActivateCollision(ECollisionPart CollisionPart)
{
	if (CollisionPart == ECollisionPart::RightFoot)
	{
		if (RightFootCollisionComponent != nullptr)
		{
			RightFootCollisionComponent->ActivateCollision();
			return;
		}
	}
	Super::ActivateCollision(CollisionPart);
}

void AToughDualSwords::DeactivateCollision(ECollisionPart CollisionPart)
{
	if (CollisionPart == ECollisionPart::RightFoot)
	{
		if (RightFootCollisionComponent != nullptr)
		{
			RightFootCollisionComponent->DeactivateCollision();
			return;
		}
	}
	Super::DeactivateCollision(CollisionPart);
}

void AToughDualSwords::BeginPlay()
{
	Super::BeginPlay();

	if (RightFootCollisionComponent != nullptr)
	{
		RightFootCollisionComponent->OnHitDelegate.BindUObject(this, &AToughDualSwords::OnHitEvent);
		RightFootCollisionComponent->AddActorToIgnore(GetOwner());
	}
}
