// Copyright Epic Games, Inc. All Rights Reserved.


#include "MasterAI.h"

#include "MeleeCombatSystemCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "BaseWeapon.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Interactable.h"
#include "EngineUtils.h"
#include "CombatComponent.h"
#include <Kismet/GameplayStatics.h>
#include "ECharacterState.h"
#include "StateManager.h"
#include "StatsComponent.h"
#include "TargetingComponent.h"
#include "Components/WidgetComponent.h"
#include <Perception/AISense_Damage.h>
#include <AIController.h>
#include "BrainComponent.h"
#include "EquipmentComponent.h"
//#include "BehaviorTree/BehaviorTreeComponent.h"


// Sets default values
AMasterAI::AMasterAI()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	// Components initialization
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	StateManager = CreateDefaultSubobject<UStateManager>(TEXT("State Manager"));
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));
	AttackTimeline = CreateDefaultSubobject<UTimelineComponent>("Attack Timeline Component");

	// Lock On Widget
	LockOnWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockOn Widget Component"));
	LockOnWidgetComponent->SetupAttachment(RootComponent);
	LockOnWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	LockOnWidgetComponent->SetDrawSize(FVector2D(14.0f, 14.0f));
}



//////////////////////////////////////////////////////////////////////////
// AMasterAI


void AMasterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AMasterAI::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	OnTakeAnyDamage.AddDynamic(this, &AMasterAI::TakeAnyDamage);
	if (StateManager != nullptr)
	{
		StateManager->OnStateBeginEvent.AddUObject(this, &AMasterAI::SwitchOnBeginStates);
		StateManager->OnStateEndEvent.AddUObject(this, &AMasterAI::SwitchOnEndStates);
		StateManager->OnActionBeginEvent.AddUObject(this, &AMasterAI::SwitchOnBeginAction);
	}
	if (StatsComponent != nullptr)
	{
		StatsComponent->OnCurrentStatValueUpdated.AddDynamic(this, &AMasterAI::CheckDeath);
	}

	AttackUpdateFunction.BindDynamic(this, &AMasterAI::UpdateRotationToTarget);
	if (AttackTimelineFloatCurve)
	{
		AttackTimeline->AddInterpFloat(AttackTimelineFloatCurve, AttackUpdateFunction);
	}

	LockOnWidgetComponent->SetVisibility(false);


	if (EquipmentComponent != nullptr)
	{
		EquipmentComponent->InitializeEquipment();
	}
	if (StatsComponent != nullptr)
	{
		StatsComponent->InitializeStats();
	}

	// Spawn immediately or remain invisible and inactive
	if (bSpawnImmediately)
	{
		PerformSpawn();
	}
	else
	{
		ToggleActor(false);
	}
}

void AMasterAI::SetMovementSpeedMode(EMovementSpeedMode NewMode)
{
	if (MovementSpeedMode != NewMode)
	{
		float& MaxWalkSpeed = GetCharacterMovement()->MaxWalkSpeed;
		switch (NewMode)
		{
		case EMovementSpeedMode::Walking:
			MaxWalkSpeed = WalkSpeed;
			break;
		case EMovementSpeedMode::Jogging:
			MaxWalkSpeed = JogSpeed;
			break;
		case EMovementSpeedMode::Sprinting:
			MaxWalkSpeed = SprintSpeed;
		}
		MovementSpeedMode = NewMode;
	}
}

bool AMasterAI::CanReceiveHitReaction() const
{
	if (StateManager == nullptr)
		return false;

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));

	if (HitReactionFrontMontage != nullptr && !StateManager->IsCurrentStateEqualToAny(States) && !bHFramesEnabled)
	{
		return true;
	}
	return false;
}


TArray<UAnimMontage*> AMasterAI::GetMontageByTag(FGameplayTag Action) const
{
	if (Action.GetTagName() == "Character.Action.FrontHitReaction")
	{
		return TArray<UAnimMontage*> { HitReactionFrontMontage };
	}
	else if (Action.GetTagName() == "Character.Action.BackHitReaction")
	{
		return TArray<UAnimMontage*> { HitReactionBackMontage };
	}
	else if (Action.GetTagName() == "Character.Action.KnockdownReaction")
	{
		return TArray<UAnimMontage*> { KnockDownMontage };
	}

	/////////////////////////////////

	if (CombatComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("CombatComponent is null"));
		return TArray<UAnimMontage*>();
	}

	// Weapon animations
	if (CombatComponent->GetMainWeapon() != nullptr)
	{
		if (Action.GetTagName() == "Character.Action.Attack.LightAttack" ||
			Action.GetTagName() == "Character.Action.Attack.StrongAttack" ||
			Action.GetTagName() == "Character.Action.Attack.ChargedAttack" ||
			Action.GetTagName() == "Character.Action.Attack.FallingAttack" ||
			Action.GetTagName() == "Character.Action.Attack.SprintAttack")
		{
			return CombatComponent->GetMainWeapon()->GetActionMontages(Action);
		}
		else if (Action.GetTagName() == "Character.Action.EnterCombat")
		{
			return TArray<UAnimMontage*> { CombatComponent->GetMainWeapon()->GetEnterCombatAnimMontage() };
		}
		else if (Action.GetTagName() == "Character.Action.ExitCombat")
		{
			return TArray<UAnimMontage*> { CombatComponent->GetMainWeapon()->GetExitCombatAnimMontage() };
		}
		else if (Action.GetTagName() == "Character.Action.ToggleCombat")
		{
			if (CombatComponent->IsCombatEnabled())
			{
				return TArray<UAnimMontage*> { CombatComponent->GetMainWeapon()->GetExitCombatAnimMontage() };
			}
			else
			{
				return TArray<UAnimMontage*> { CombatComponent->GetMainWeapon()->GetEnterCombatAnimMontage() };
			}
		}

	}

	/////////////////////////////////


	// Shield animation
	if (CombatComponent->IsShieldEquipped())
	{
		if (Action.GetTagName() == "Character.Action.BlockReaction")
		{
			return TArray<UAnimMontage*>(CombatComponent->GetMainWeapon()->GetActionMontages(FGameplayTag::RequestGameplayTag("Character.Action.BlockReaction")));
		}
	}

	return TArray<UAnimMontage*>();
}


void AMasterAI::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CanReceiveDamage())
		return;

	// If damage causer is not the player then do not proceed damage
	if (Cast<AMeleeCombatSystemCharacter>(DamageCauser->GetOwner()) == nullptr)
	{
		return;
	}

	const UAttackDamageType* AttackDamageType = Cast<UAttackDamageType>(DamageType);
	EDamageType AttackType = EDamageType::None;

	if (AttackDamageType != nullptr)
	{
		AttackType = AttackDamageType->GetDamageType();
	}

	FindIfBehind(DamageCauser);

	if (StatsComponent != nullptr)
	{
		StatsComponent->TakeDamage(Damage);
	}

	UAISense_Damage::ReportDamageEvent(GetWorld(), this, InstigatedBy->GetPawn(), Damage, GetActorLocation(), GetActorLocation());

	ApplyImpactEffects(DamagedActor);

	if (CanReceiveHitReaction())
	{
		ApplyHitReaction(AttackType);
	}
}

void AMasterAI::FindIfBehind(AActor* DamageCauser)
{
	FVector MainCharacterLocation = DamageCauser->GetActorLocation();
	FVector TargetLocation = this->GetActorLocation();
	FVector ForwardVector = this->GetActorForwardVector();
	FVector DirectionToMainCharacter = (MainCharacterLocation - TargetLocation).GetSafeNormal();
	float DotProduct = FVector::DotProduct(DirectionToMainCharacter, ForwardVector);
	if (DotProduct >= -0.05 && DotProduct <= 1)
	{
		bHitFront = true;
	}
	else
	{
		bHitFront = false;
	}
}

void AMasterAI::ApplyImpactEffects(AActor* DamagedActor)
{
	if (GuardianImpactSoundCue != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GuardianImpactSoundCue, DamagedActor->GetActorLocation());
	}
	if (GuardianImpactParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, GuardianImpactParticles, DamagedActor->GetActorLocation());
	}
}

void AMasterAI::CheckDeath(EStats Type, float Value)
{
	if (Type == EStats::Health)
	{
		if (StatsComponent->GetCurrentStatValue(EStats::Health) <= 0)
		{
			Die();
		}
	}
}

void AMasterAI::ApplyHitReaction(EDamageType DamageType)
{
	switch (DamageType)
	{
	case EDamageType::MeleeDamage:
		PerformHitStun();
		break;
	case EDamageType::KnockDownDamage:
		PerformKnockDown();
		break;
	case EDamageType::None:
	default:
		return;
	}

}

void AMasterAI::PerformHitStun()
{
	if (bHitFront)
	{
		PerformAction(FGameplayTag::RequestGameplayTag("Character.Action.FrontHitReaction"),
			FGameplayTag::RequestGameplayTag("Character.State.Disabled"), true);
	}
	else
	{
		PerformAction(FGameplayTag::RequestGameplayTag("Character.Action.BackHitReaction"),
			FGameplayTag::RequestGameplayTag("Character.State.Disabled"), true);
	}
}

void AMasterAI::PerformKnockDown()
{
	PerformAction(FGameplayTag::RequestGameplayTag("Character.Action.KnockdownReaction"),
		FGameplayTag::RequestGameplayTag("Character.State.Disabled"), true);
}

void AMasterAI::RotateToTarget()
{
	AttackTimeline->PlayFromStart();

}

void AMasterAI::StopRotatingToTarget()
{
	AttackTimeline->Stop();
}

void AMasterAI::UpdateRotationToTarget(float Value)
{
	if (TargetActor == nullptr)
		return;

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	// Update camera rotation to the target
	FVector CharacterLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator NewRotation = (TargetLocation - CharacterLocation).Rotation();
	FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 90.0f);
	InterpRotation.Pitch = GetActorRotation().Pitch;
	SetActorRotation(InterpRotation);
}

void AMasterAI::ToggleActor(bool bEnable)
{
	if (bEnable)
	{
		SetActorHiddenInGame(false);
		SetActorEnableCollision(true);
		SetActorTickEnabled(true);
		if (auto MainWeapon = CombatComponent->GetMainWeapon())
		{
			MainWeapon->SetActorHiddenInGame(false);
			MainWeapon->SetActorTickEnabled(true);
		}
		Cast<AAIController>(Controller)->GetBrainComponent()->StartLogic();
	}
	else
	{
		SetActorHiddenInGame(true);
		SetActorEnableCollision(false);
		SetActorTickEnabled(false);
		if (auto MainWeapon = CombatComponent->GetMainWeapon())
		{
			MainWeapon->SetActorHiddenInGame(true);
			MainWeapon->SetActorTickEnabled(false);
		}
		Cast<AAIController>(Controller)->GetBrainComponent()->StopLogic("Disabled actor");
	}
}

bool AMasterAI::CanBeTargeted()
{
	FGameplayTagContainer IsDead(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
	return !StateManager->IsCurrentStateEqualToAny(IsDead);
}

void AMasterAI::OnTargeted(bool bIsTargeted)
{
	if (LockOnWidgetComponent != nullptr)
	{
		LockOnWidgetComponent->SetVisibility(bIsTargeted);
	}
}


bool AMasterAI::CanPerformAttack() const
{
	if (StateManager == nullptr)
		return false;


	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Disabled")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dodging")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.GeneralActionState")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));

	if (StatsComponent != nullptr)
	{
		if (StatsComponent->GetCurrentStatValue(EStats::Stamina) < 10)
			return false;
	}

	return !StateManager->IsCurrentStateEqualToAny(States) && !GetCharacterMovement()->IsFalling();
}


void AMasterAI::ContinueAttack(FGameplayTag AttackType)
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->PerformNextAttack(AttackType);
	}
}

void AMasterAI::ResetAttack()
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->ResetAttack();
	}
}

FRotator AMasterAI::GetDesiredRotation() const
{
	UCharacterMovementComponent* Movement = GetCharacterMovement();
	if (Movement == nullptr) return FRotator();

	if (!Movement->GetLastInputVector().Equals(FVector(0.0f, 0.0f, 0.0f), 0.001f))
	{
		return FRotator(Movement->GetLastInputVector().Rotation());
	}
	else
	{
		return GetActorRotation();
	}
}

void AMasterAI::ResetCombat()
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->ResetCombat();
	}
}

void AMasterAI::EnableRagdoll()
{
	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PelvisBoneName, 1.0f);
}

void AMasterAI::ApplyHitReactionPhysics(float InitialSpeed)
{
	FVector Speed = GetActorForwardVector() * -InitialSpeed;
	if (!bHitFront)
	{
		Speed = GetActorForwardVector() * InitialSpeed;
	}
	GetMesh()->SetPhysicsLinearVelocity(Speed);
}

void AMasterAI::PerformSpawn(bool bRandomIndex)
{
	if (!bSpawnImmediately)
	{
		ToggleActor(true);
	}
	if (SpawnMontages.Num() > 0)
	{
		if (bRandomIndex)
		{
			PlayAnimMontage(SpawnMontages[FMath::RandRange(0, SpawnMontages.Num() - 1)]);
		}
		else
		{
			PlayAnimMontage(SpawnMontages[0]);
		}
	}
}

void AMasterAI::Die(bool bRandomIndex)
{
	UE_LOG(LogTemp, Display, TEXT("%s died"), *GetActorNameOrLabel());

	StateManager->SetCurrentState(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	// Disable AI logic
	if (AAIController* AIController = Cast<AAIController>(GetController()))
	{
		if (AIController != nullptr && AIController->GetBrainComponent() != nullptr)
		{
			AIController->GetBrainComponent()->StopLogic("Dead");
		}
	}

	if (DeathMontages.Num() > 0)
	{
		if (bRandomIndex)
		{
			PlayAnimMontage(DeathMontages[FMath::RandRange(0, DeathMontages.Num() - 1)]);
		}
		else
		{
			PlayAnimMontage(DeathMontages[0]);
		}
	}
	else
	{
		EnableRagdoll();
		ApplyHitReactionPhysics(HitReactionInitialSpeed);
	}

	// Destroy character
	FTimerHandle CharacterDestroyTimer;
	GetWorld()->GetTimerManager().SetTimer(CharacterDestroyTimer,
		[this]() {Destroy(); },
		DeathDestroyTimer, false);

	// Destroy main weapon
	if (CombatComponent->GetMainWeapon() != nullptr)
	{
		CombatComponent->GetMainWeapon()->SetActorEnableCollision(false);
		CombatComponent->GetMainWeapon()->SimulateWeaponPhysics();
		FTimerHandle WeaponDestroyTimer;
		GetWorld()->GetTimerManager().SetTimer(WeaponDestroyTimer,
			[this]() {CombatComponent->GetMainWeapon()->Destroy(); },
			WeaponDeathDestroyTimer, false);
	}
}

void AMasterAI::OnTargetSet(AActor* Target)
{
	TargetActor = Target;
}


bool AMasterAI::CanReceiveDamage() const
{
	if (StateManager != nullptr)
	{
		return StateManager->GetCurrentState() != FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")) &&
			!bIFramesEnabled;
	}
	return false;
}

void AMasterAI::SetInvincibleFrames(bool bEnableIFrames)
{
	bIFramesEnabled = bEnableIFrames;
}

void AMasterAI::SetHyperarmorFrames(bool bEnableHFrames)
{
	bHFramesEnabled = bEnableHFrames;
}


void AMasterAI::UpdateHealthPotionAmount(int Amount)
{
}


float AMasterAI::PerformAction(FGameplayTag Action, FGameplayTag State, bool bRandomIndex, int MontageIndex)
{
	auto Montages = GetMontageByTag(Action);

	if (Montages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No montages for action: %s"), *Action.ToString());
		return 0;
	}

	UAnimMontage* ActionMontage = Montages.IsValidIndex(MontageIndex) ? Montages[MontageIndex] : Montages[0];

	if (bRandomIndex)
	{
		ActionMontage = Montages[FMath::RandRange(0, Montages.Num() - 1)];
	}


	SetCurrentAction(Action);
	SetCurrentState(State);
	return PlayAnimMontage(ActionMontage);
}

float AMasterAI::PerformCustomAction(FGameplayTag ActionTag, FGameplayTag StateTag, UAnimMontage* InMontage, float MontagePlayRate, bool bAutoReset)
{
	SetCurrentAction(ActionTag);
	SetCurrentState(StateTag);

	return GetMesh()->GetAnimInstance()->Montage_Play(InMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, 0.0f, bAutoReset);
}

float AMasterAI::PerformAttack(FGameplayTag AttackTag, int MontageIndex, bool bRandomIndex)
{
	if (CombatComponent != nullptr)
	{
		return CombatComponent->PerformAttack(AttackTag, MontageIndex, bRandomIndex);
	}
	return 0;
}

bool AMasterAI::CanPerformAnimation() const
{
	if (StateManager == nullptr)
		return false;

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Attacking")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Disabled")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dodging")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.GeneralActionState")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));

	return !StateManager->IsCurrentStateEqualToAny(States) && !GetCharacterMovement()->IsFalling();
}

FGameplayTag AMasterAI::GetCurrentState() const
{
	if (StateManager != nullptr)
	{
		return StateManager->GetCurrentState();
	}
	return FGameplayTag::EmptyTag;
}

void AMasterAI::SetCurrentState(FGameplayTag NewState)
{
	if (StateManager != nullptr)
	{
		StateManager->SetCurrentState(NewState);
	}
}

void AMasterAI::ResetState()
{
	if (StateManager != nullptr)
	{
		StateManager->ResetState();
	}
}

FGameplayTag AMasterAI::GetCurrentAction() const
{
	if (StateManager != nullptr)
	{
		return StateManager->GetCurrentAction();
	}
	return FGameplayTag::EmptyTag;
}

void AMasterAI::SetCurrentAction(FGameplayTag NewAction)
{
	if (StateManager != nullptr)
	{
		StateManager->SetCurrentAction(NewAction);
	}
}

void AMasterAI::ResetAction()
{
	if (StateManager != nullptr)
	{
		StateManager->ResetAction();
	}
}



void AMasterAI::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}


void AMasterAI::LightAttack()
{
	if (CombatComponent == nullptr) return;

	// Falling attack
	if (GetCharacterMovement()->IsFalling())
	{
		CombatComponent->PerformAttack
		(
			FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.FallingAttack")),
			CombatComponent->GetAttackCount(), false
		);
	}

	if (CanPerformAttack())
	{
		CombatComponent->PerformAttack
		(
			FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.LightAttack")),
			CombatComponent->GetAttackCount(), false
		);
	}

}


void AMasterAI::HeavyAndSprintAttack()
{
	if (CombatComponent == nullptr) return;
	if (CanPerformAttack())
	{
		if (bIsSprintAttack && GetCharacterMovement()->Velocity.Size() >= SprintSpeed - 0.01f)
		{
			CombatComponent->PerformAttack
			(
				FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.SprintAttack")),
				CombatComponent->GetAttackCount(), false
			);
		}
		else
		{
			CombatComponent->PerformAttack
			(
				FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.StrongAttack")),
				CombatComponent->GetAttackCount(), false
			);
		}
	}

}

void AMasterAI::ChargedAttack()
{
	if (CombatComponent == nullptr) return;
	if (CanPerformAttack())
	{
		CombatComponent->PerformAttack
		(
			FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack.ChargedAttack")),
			CombatComponent->GetAttackCount(), false
		);
	}
}

bool AMasterAI::ToggleCombat()
{
	if (CombatComponent == nullptr) return false;

	if (CanPerformAnimation())
	{
		CombatComponent->ToggleCombat();
		return true;
	}
	return false;
}


void AMasterAI::Jump()
{
	if (CanPerformAnimation())
	{
		Super::Jump();

		if (StatsComponent != nullptr)
		{
			StatsComponent->ModifyCurrentStatValue(EStats::Stamina, -10);
		}
	}
}

void AMasterAI::ToggleWalk()
{
	if (MovementSpeedMode == EMovementSpeedMode::Jogging)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Walking);
	}
	else if (MovementSpeedMode == EMovementSpeedMode::Walking)
	{
		SetMovementSpeedMode(EMovementSpeedMode::Jogging);
	}
}

void AMasterAI::Sprint()
{
	if (StatsComponent != nullptr)
	{
		if (StatsComponent->GetCurrentStatValue(EStats::Stamina) <= 10)
		{
			return;
		}
	}
	SetMovementSpeedMode(EMovementSpeedMode::Sprinting);
	GetWorld()->GetTimerManager().SetTimer(ChargeAttackTimerHandle, [this]() { bIsSprintAttack = true; }, 1.0f, false);
}


void AMasterAI::StopSprint()
{
	SetMovementSpeedMode(EMovementSpeedMode::Jogging);
	GetWorld()->GetTimerManager().ClearTimer(ChargeAttackTimerHandle);
	bIsSprintAttack = false;
}

void AMasterAI::SwitchOnBeginStates(FGameplayTag State)
{
	if (State.GetTagName() == "Character.State.Attacking")
	{
		RotateToTarget();
	}
	else if (State.GetTagName() == "Character.State.Dodging")
	{

	}
	else if (State.GetTagName() == "Character.State.TogglingCombat")
	{

	}
	else if (State.GetTagName() == "Character.State.Dead")
	{
		StateManager->OnStateBeginEvent.Clear();
	}
	else if (State.GetTagName() == "Character.State.GeneralActionState")
	{

	}
	else if (State.GetTagName() == "Character.State.Disabled")
	{

	}
	else if (State.GetTagName() == "Character.State.Nothing")
	{

	}
}

void AMasterAI::SwitchOnEndStates(FGameplayTag State)
{
	if (State.GetTagName() == "Character.State.Attacking")
	{
		StopRotatingToTarget();
	}
	else if (State.GetTagName() == "Character.State.Dodging")
	{

	}
	else if (State.GetTagName() == "Character.State.TogglingCombat")
	{

	}
	else if (State.GetTagName() == "Character.State.Dead")
	{

	}
	else if (State.GetTagName() == "Character.State.GeneralActionState")
	{

	}
	else if (State.GetTagName() == "Character.State.Disabled")
	{

	}
	else if (State.GetTagName() == "Character.State.Nothing")
	{

	}
}

void AMasterAI::SwitchOnBeginAction(FGameplayTag Action)
{
	if (Action.GetTagName() == "Character.Action.Attack.LightAttack")
	{

	}
	else if (Action.GetTagName() == "Character.Action.Attack.StrongAttack")
	{

	}
	else if (Action.GetTagName() == "Character.Action.Attack.ChargedAttack")
	{

	}
	else if (Action.GetTagName() == "Character.Action.Attack.FallingAttack")
	{

	}
	else if (Action.GetTagName() == "Character.Action.Attack.SprintAttack")
	{

	}
	else if (Action.GetTagName() == "Character.Action.Dodge")
	{

	}
	else if (Action.GetTagName() == "Character.Action.EnterCombat")
	{

	}
	else if (Action.GetTagName() == "Character.Action.ExitCombat")
	{
		UE_LOG(LogTemp, Display, TEXT("Exit combat"));
		CombatComponent->SetIsCombatEnabled(false);
	}
	else if (Action.GetTagName() == "Character.Action.Nothing")
	{

	}
}
