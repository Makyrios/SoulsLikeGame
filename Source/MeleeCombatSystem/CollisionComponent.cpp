// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UCollisionComponent::UCollisionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCollisionComponent::BeginPlay()
{
	Super::BeginPlay();
	
}


// Called every frame
void UCollisionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsCollisionEnabled)
	{
		CollisionTrace();
	}
}

void UCollisionComponent::SetCollisionMesh(UPrimitiveComponent* Value)
{
	if (bShouldUseWeaponMesh)
	{
		CollisionMeshComponent = Value;
	}
	// Use character mesh
	else
	{
		CollisionMeshComponent = Cast<ACharacter>(GetOwner()->GetOwner())->GetMesh();
	}
}

void UCollisionComponent::ActivateCollision()
{
	ClearHitActors();
	bIsCollisionEnabled = true;
}

void UCollisionComponent::CollisionTrace()
{
	if (CollisionMeshComponent == nullptr) return;
	FVector Start = CollisionMeshComponent->GetSocketLocation(StartSocketName);
	FVector End = CollisionMeshComponent->GetSocketLocation(EndSocketName);

	TArray<FHitResult> Hit;
	bool bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, TraceRadius,
		ObjectTypesArray, false, ActorsToIgnore,
		EDrawDebugTrace::None, Hit, true);
	if (bIsHit)
	{
		for (FHitResult HitResult : Hit)
		{
			AActor* HitActor = HitResult.GetActor();
			if (!AlreadyHitActors.Contains(HitActor))
			{
				AlreadyHitActors.Add(HitActor);
				if (OnHitDelegate.IsBound())
					OnHitDelegate.Execute(HitResult);
			}
		}
	}
}

