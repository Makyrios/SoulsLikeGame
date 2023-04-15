// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Targetable.h"
#include "CombatComponent.h"
#include "StateControl.h"
#include "EMovementSpeedMode.h"

// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	OwnerController = OwnerCharacter->GetController();
	FollowCamera = OwnerCharacter->FindComponentByClass<UCameraComponent>();
	CameraBoom = OwnerCharacter->FindComponentByClass<USpringArmComponent>();
	CombatComponent = OwnerCharacter->FindComponentByClass<UCombatComponent>();

	ActorsToIgnore.Add(GetOwner());
}


// Called every frame
void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateTargetingControlRotation(DeltaTime);
}

void UTargetingComponent::EnableLockOn()
{
	if (FindTarget(&TargetActor))
	{
		SetIsTargeting(true);
		SetRotationMode(ERotationMode::OrientToCamera);
		//UpdateRotationMode();
	}
}

void UTargetingComponent::DisableLockOn()
{
	SetIsTargeting(false);
	SetTargetActor(nullptr);
	SetRotationMode(ERotationMode::OrientToMovement);
	//UpdateRotationMode();
}

void UTargetingComponent::ToggleLockOn()
{
	if (!IsTargeting())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enable LockOn"));
		EnableLockOn();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enable LockOn"));
		DisableLockOn();
	}
}

void UTargetingComponent::SetIsTargeting(bool Value)
{
	bIsTargeting = Value;
	if (TargetActor != nullptr)
	{
		ITargetable* Target = Cast<ITargetable>(TargetActor);
		if (Target != nullptr)
		{
			Target->OnTargeted(Value);
		}
	}
}

void UTargetingComponent::SetRotationMode(ERotationMode Mode)
{
	CurrentRotationMode = Mode;
	switch (Mode)
	{
	case ERotationMode::OrientToCamera:
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = true;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		break;
	case ERotationMode::OrientToMovement:
		OwnerCharacter->bUseControllerRotationYaw = false;
		OwnerCharacter->GetCharacterMovement()->bUseControllerDesiredRotation = false;
		OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement = true;
	}
}

//void UTargetingComponent::UpdateRotationMode()
//{
//	if (IStateControl* State = Cast<IStateControl>(GetOwner()))
//	{
//		if (State->GetMovementSpeedMode() == EMovementSpeedMode::Sprinting &&
//			CombatComponent->IsCombatEnabled() &&
//			IsTargeting())
//		{
//			UE_LOG(LogTemp, Warning, TEXT("UpdateRotationMode"));
//			SetRotationMode(ERotationMode::OrientToCamera);
//		}
//		else
//		{
//			SetRotationMode(ERotationMode::OrientToMovement);
//		}
//	}
//}

bool UTargetingComponent::FindTarget(AActor** FoundTarget)
{
	if (FollowCamera == nullptr) return false;

	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start + (FollowCamera->GetForwardVector() * TargetingDistance);

	TArray<FHitResult> Hit;
	bool bIsHit = UKismetSystemLibrary::SphereTraceMultiForObjects(this, Start, End, TargetingRadius,
		ObjectTypesArray, false, ActorsToIgnore,
		EDrawDebugTrace::None, Hit, true);


	if (bIsHit)
	{
		if (CanTargetActor(Hit[0].GetActor()))
		{
			*FoundTarget = Hit[0].GetActor();
			return true;
		}
	}
	return false;
}

void UTargetingComponent::UpdateTargetingControlRotation(float DeltaTime)
{
	if (!IsTargeting() || TargetActor == nullptr || OwnerController == nullptr)
	{
		return;
	}

	if (Cast<IStateControl>(OwnerCharacter)->GetCurrentState() == FGameplayTag::RequestGameplayTag("Character.State.Dead"))
	{
		DisableLockOn();
		PrimaryComponentTick.bCanEverTick = false;
	}

	if (!CanTargetActor(TargetActor))
	{
		DisableLockOn();
		return;
	}

	// Update camera orientation
	if (CombatComponent != nullptr)
	{
		if (CombatComponent->IsCombatEnabled())
		{
			SetRotationMode(ERotationMode::OrientToCamera);
		}
		else
		{
			SetRotationMode(ERotationMode::OrientToMovement);
		}
	}

	// Update camera rotation to the target
	FVector CharacterLocation = GetOwner()->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	TargetLocation.Z -= 100;
	FRotator NewRotation = (TargetLocation - CharacterLocation).Rotation();
	FRotator InterpolatedRotator = FMath::RInterpTo(OwnerCharacter->GetActorRotation(), NewRotation, DeltaTime, CameraLockInterpolationSpeed);
	InterpolatedRotator.Pitch = FMath::Clamp(InterpolatedRotator.Pitch, -25, 30);
	OwnerController->SetControlRotation(InterpolatedRotator);

}

bool UTargetingComponent::CanTargetActor(AActor* Actor) const
{
	if (auto Target = Cast<ITargetable>(Actor))
	{
		if (Target->CanBeTargeted() &&
			FVector::Dist(GetOwner()->GetActorLocation(), Actor->GetActorLocation()) <= TargetingDistance)
		{
			return true;
		}
	}
	return false;
}

