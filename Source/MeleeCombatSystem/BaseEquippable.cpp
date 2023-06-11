// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEquippable.h"
#include "GameFramework/Character.h"

// Sets default values
ABaseEquippable::ABaseEquippable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemStaticMesh"));
	RootComponent = ItemStaticMesh;

	ItemSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemSkeletalMesh"));
	ItemSkeletalMesh->SetupAttachment(RootComponent);

	SetActorEnableCollision(false);
}

// Called when the game starts or when spawned
void ABaseEquippable::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEquippable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseEquippable::OnEquipped()
{
	bIsEquipped = true;
}

void ABaseEquippable::OnUnequipped()
{
	bIsEquipped = false;
}

bool ABaseEquippable::IsEquipped() const
{
	return bIsEquipped;
}

void ABaseEquippable::SetIsEquipped(bool Value)
{
	bIsEquipped = Value;
}

void ABaseEquippable::AttachActor(FName SocketName)
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr)
	{
		this->AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
	}
}

void ABaseEquippable::DetachActor()
{
	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (Character != nullptr)
	{
		this->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	}
}

void ABaseEquippable::PerformItemAction()
{
}

void ABaseEquippable::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = OwnedGameplayTags;
}

