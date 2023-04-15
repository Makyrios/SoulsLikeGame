// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterPose.h"
#include "GameFramework/Character.h"
#include "StatsComponent.h"

AMasterPose::AMasterPose()
{
}

void AMasterPose::AttachActor(FName SocketName)
{
	if (GetSkeletalMesh() == nullptr || GetOwner() == nullptr) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());

	if (OwnerChar == nullptr) return;

	GetSkeletalMesh()->AttachToComponent(OwnerChar->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);


	USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetSkeletalMesh());
	if (SkeletalMesh != nullptr)
	{
		SkeletalMesh->SetLeaderPoseComponent(OwnerChar->GetMesh());
	}
}

void AMasterPose::DetachActor()
{
	if (GetSkeletalMesh() == nullptr || GetOwner() == nullptr) return;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());

	if (OwnerChar == nullptr) return;

	GetSkeletalMesh()->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);


	/*USkeletalMeshComponent* SkeletalMesh = Cast<USkeletalMeshComponent>(GetSkeletalMesh());
	if (SkeletalMesh != nullptr)
	{
		SkeletalMesh->SetLeaderPoseComponent(OwnerChar->GetMesh());
	}*/
}

void AMasterPose::OnEquipped()
{
	Super::OnEquipped();

	AttachActor("");

	UStatsComponent* StatsComponent = GetOwner()->FindComponentByClass<UStatsComponent>();
	if (StatsComponent == nullptr)
	{
		return;
	}

	StatsComponent->ModifyCurrentStatValue(EStats::Armor, ArmorValue, false);
}

void AMasterPose::OnUnequipped()
{
	UStatsComponent* StatsComponent = GetOwner()->FindComponentByClass<UStatsComponent>();
	if (StatsComponent == nullptr) return;

	("");

	StatsComponent->ModifyCurrentStatValue(EStats::Armor, -ArmorValue, false);
}
