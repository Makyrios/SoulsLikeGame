// Copyright Epic Games, Inc. All Rights Reserved.

#include "MeleeCombatSystemCharacter.h"
#include "Camera/CameraComponent.h"
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
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Components/TimelineComponent.h"
#include "CombatPlayerController.h"
#include "EquipmentComponent.h"
#include "InventoryComponent.h"
#include "SwordAndShield.h"

//////////////////////////////////////////////////////////////////////////
// AMeleeCombatSystemCharacter

AMeleeCombatSystemCharacter::AMeleeCombatSystemCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

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

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	StateManager = CreateDefaultSubobject<UStateManager>(TEXT("State Manager"));
	StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("Targeting Component"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment Component"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory Component");
	AIPerceptionSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>("AI Perception Source");
	AttackTimeline = CreateDefaultSubobject<UTimelineComponent>("Attack Timeline Component");


	// EVENTS

	//OnTakePointDamage.AddDynamic(this, &AMeleeCombatSystemCharacter::TakePointDamage);
	OnTakeAnyDamage.AddDynamic(this, &AMeleeCombatSystemCharacter::TakeAnyDamage);
	if (StateManager != nullptr)
	{
		StateManager->OnStateBeginEvent.AddUObject(this, &AMeleeCombatSystemCharacter::SwitchOnBeginStates);
		StateManager->OnStateEndEvent.AddUObject(this, &AMeleeCombatSystemCharacter::SwitchOnEndStates);
		//StateManager->OnActionBeginEvent.AddUObject(this, &AMeleeCombatSystemCharacter::SwitchOnBeginActions);
		//StateManager->OnActionEndEvent.AddUObject(this, &AMeleeCombatSystemCharacter::SwitchOnEndActions);
	}
	if (StatsComponent != nullptr)
	{
		StatsComponent->OnCurrentStatValueUpdated.AddDynamic(this, &AMeleeCombatSystemCharacter::CheckDeath);
	}
}

void AMeleeCombatSystemCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeleeCombatSystemCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	AttackUpdateFunction.BindDynamic(this, &AMeleeCombatSystemCharacter::UpdateRotationToTarget);


	if (AttackTimelineFloatCurve)
	{
		AttackTimeline->AddInterpFloat(AttackTimelineFloatCurve, AttackUpdateFunction);
	}

	
	EquipmentComponent->InitializeEquipment();
	InventoryComponent->InitializeWeapons();
	StatsComponent->InitializeStats();
}

void AMeleeCombatSystemCharacter::SetMovementSpeedMode(EMovementSpeedMode NewMode)
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

bool AMeleeCombatSystemCharacter::CanReceiveHitReaction() const
{
	check(StateManager);

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));

	if (HitReactionFrontMontage != nullptr && !StateManager->IsCurrentStateEqualToAny(States) && !bHFramesEnabled)
	{
		return true;
	}
	return false;
}

float AMeleeCombatSystemCharacter::GetActionStaminaCost(FGameplayTag Action) const
{
	if (Action.RequestDirectParent() == FGameplayTag::RequestGameplayTag("Character.Action.Attack"))
	{
		return CombatComponent->GetMainWeapon()->GetActionStatCost();
	}

	if (ActionsStaminaCost.Find(Action) != nullptr)
	{
		return *ActionsStaminaCost.Find(Action);
	}
	else
	{
		return 0;
	}
}

TArray<UAnimMontage*> AMeleeCombatSystemCharacter::GetMontageByTag(FGameplayTag Action) const
{
	if (Action.GetTagName() == "Character.Action.Dodge")
	{
		return TArray<UAnimMontage*> { DodgeMontage };
	}
	else if (Action.GetTagName() == "Character.Action.FrontHitReaction")
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

		// Shield animation
		else if (CombatComponent->GetMainWeapon()->CanBlock())
		{
			if (Action.GetTagName() == "Character.Action.BlockReaction")
			{
				return TArray<UAnimMontage*>(CombatComponent->GetMainWeapon()->GetActionMontages(FGameplayTag::RequestGameplayTag("Character.Action.BlockReaction")));
			}
		}

	}



	return TArray<UAnimMontage*>();
}

float AMeleeCombatSystemCharacter::PerformAction(FGameplayTag Action, FGameplayTag State, bool bRandomIndex, int MontageIndex)
{
	auto Montages = GetMontageByTag(Action);

	if (Montages.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No montages for action: %s"), *Action.ToString());
		return 0.f;
	}

	UAnimMontage* ActionMontage = Montages.IsValidIndex(MontageIndex) ? Montages[MontageIndex] : Montages[0];

	if (bRandomIndex)
	{
		ActionMontage = Montages[FMath::RandRange(0, Montages.Num() - 1)];
	}

	
	SetCurrentAction(Action);
	SetCurrentState(State);
	if (StatsComponent != nullptr)
	{
		StatsComponent->ModifyCurrentStatValue(EStats::Stamina, -GetActionStaminaCost(Action));
	}
	return PlayAnimMontage(ActionMontage);
}

float AMeleeCombatSystemCharacter::PerformCustomAction(FGameplayTag ActionTag, FGameplayTag StateTag, UAnimMontage* InMontage, float MontagePlayRate, bool bAutoReset)
{
	SetCurrentAction(ActionTag);
	SetCurrentState(StateTag);
	if (StatsComponent != nullptr)
	{
		StatsComponent->ModifyCurrentStatValue(EStats::Stamina, -GetActionStaminaCost(ActionTag));
	}
	return GetMesh()->GetAnimInstance()->Montage_Play(InMontage, MontagePlayRate, EMontagePlayReturnType::MontageLength, 0.0f, bAutoReset);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AMeleeCombatSystemCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AMeleeCombatSystemCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMeleeCombatSystemCharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMeleeCombatSystemCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMeleeCombatSystemCharacter::Look);

		//Toggle combat
		EnhancedInputComponent->BindAction(ToggleCombatAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::ToggleCombat);

		//Interact
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::Interact);

		//Light attack
		EnhancedInputComponent->BindAction(LightAttackAction, ETriggerEvent::Completed, this, &AMeleeCombatSystemCharacter::LightAttack);

		//Charge attack
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::HeavyAndSprintAttack);

		//Charged attack
		EnhancedInputComponent->BindAction(ChargedAttackAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::ChargedAttack);

		//Dodge
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::Dodge);

		//ToggleWalk
		EnhancedInputComponent->BindAction(ToggleWalkAction, ETriggerEvent::Completed, this, &AMeleeCombatSystemCharacter::ToggleWalk);

		//Sprint
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMeleeCombatSystemCharacter::Sprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMeleeCombatSystemCharacter::StopSprint);

		//Target lock on
		EnhancedInputComponent->BindAction(LockOnTargetAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::LockOnTarget);

		//Block
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::StartBlock);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &AMeleeCombatSystemCharacter::StopBlock);

		//Use item
		EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::PerformItemAction);

		//Change weapon
		EnhancedInputComponent->BindAction(ChangeWeaponAction, ETriggerEvent::Started, this, &AMeleeCombatSystemCharacter::ChangeWeapon);

		PlayerInputComponent->BindAction(TEXT("Die"), IE_Pressed, this, &AMeleeCombatSystemCharacter::RegenStamina);
	}

}


void AMeleeCombatSystemCharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!CanReceiveDamage())
		return;

	const UAttackDamageType* AttackDamageType = Cast<UAttackDamageType>(DamageType);
	EDamageType AttackType = EDamageType::None;

	if (AttackDamageType != nullptr)
	{
		AttackType = AttackDamageType->GetDamageType();
	}

	// Change bool if hit was from behind
	FindIfBehind(DamageCauser);

	if (StatsComponent != nullptr)
	{
		if (WasHitBlocked())
		{
			ASwordAndShield* SwordShield = Cast<ASwordAndShield>(CombatComponent->GetMainWeapon());
			if (SwordShield != nullptr)
			{
				float DamageReduction = Damage * SwordShield->GetDamageReductionMultiplier() * 0.01;
				StatsComponent->TakeDamage(Damage - DamageReduction);
			}
		}
		else
		{
			StatsComponent->TakeDamage(Damage);
		}
	}

	ApplyImpactEffects(DamagedActor);

	if (CanReceiveHitReaction())
	{
		ApplyHitReaction(AttackType);
	}
}

void AMeleeCombatSystemCharacter::FindIfBehind(AActor* DamageCauser)
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

void AMeleeCombatSystemCharacter::ApplyImpactEffects(AActor* DamagedActor)
{
	if (WasHitBlocked())
	{
		return;
	}
	if (GuardianImpactSoundCue != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, GuardianImpactSoundCue, DamagedActor->GetActorLocation());
	}
	if (GuardianImpactParticles != nullptr)
	{
		UGameplayStatics::SpawnEmitterAtLocation(this, GuardianImpactParticles, DamagedActor->GetActorLocation());
	}
}

void AMeleeCombatSystemCharacter::CheckDeath(EStats Type, float Value)
{
	if (Type == EStats::Health)
	{
		if (StatsComponent->GetCurrentStatValue(EStats::Health) <= 0)
		{
			Die();
		}
	}
}

void AMeleeCombatSystemCharacter::ApplyHitReaction(EDamageType DamageType)
{
	if (WasHitBlocked() && DamageType != EDamageType::KnockDownDamage)
	{
		PerformBlock();
		return;
	}

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

bool AMeleeCombatSystemCharacter::WasHitBlocked() const
{
	if (CombatComponent != nullptr)
	{
		if (CombatComponent->IsBlocking() && bHitFront)
		{
			UE_LOG(LogTemp, Display, TEXT("Hit was blocked"));
			return true;
		}
	}
	return false;
}

void AMeleeCombatSystemCharacter::PerformHitStun()
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

void AMeleeCombatSystemCharacter::PerformKnockDown()
{
	PerformAction(FGameplayTag::RequestGameplayTag("Character.Action.KnockdownReaction"),
		FGameplayTag::RequestGameplayTag("Character.State.Disabled"), true);
}


void AMeleeCombatSystemCharacter::RotateToTarget()
{
	AttackTimeline->PlayFromStart();
}

void AMeleeCombatSystemCharacter::StopRotatingToTarget()
{
	AttackTimeline->Stop();
}

void AMeleeCombatSystemCharacter::UpdateRotationToTarget(float Value)
{
	AActor* TargetActor = TargetingComponent->GetTargetActor();
	if (TargetActor == nullptr)
		return;

	float DeltaTime = UGameplayStatics::GetWorldDeltaSeconds(this);

	// Update camera rotation to the target
	FVector CharacterLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FRotator NewRotation = (TargetLocation - CharacterLocation).Rotation();
	FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), NewRotation, DeltaTime, 9.0f);
	InterpRotation.Pitch = GetActorRotation().Pitch;
	SetActorRotation(InterpRotation);
}

void AMeleeCombatSystemCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = OwnedGameplayTags;
}

float AMeleeCombatSystemCharacter::PerformAttack(FGameplayTag AttackTag, int MontageIndex, bool bRandomIndex)
{
	return CombatComponent->PerformAttack(AttackTag, MontageIndex, bRandomIndex);
}

void AMeleeCombatSystemCharacter::PerformBlock()
{
	PerformAction(FGameplayTag::RequestGameplayTag("Character.Action.BlockReaction"),
		FGameplayTag::RequestGameplayTag("Character.State.Disabled"), true);
}

void AMeleeCombatSystemCharacter::StartBlock()
{
	bIsBlockPressed = true;

	if (CombatComponent == nullptr) return;

	if (CanPerformBlock())
	{
		CombatComponent->SetBlockingState(true);
	}
}

void AMeleeCombatSystemCharacter::StopBlock()
{
	bIsBlockPressed = false;

	if (CombatComponent == nullptr) return;

	CombatComponent->SetBlockingState(false);
}

void AMeleeCombatSystemCharacter::PerformItemAction()
{
	if (CanUseItem())
	{
		EquipmentComponent->UseItemByTag(FGameplayTag::RequestGameplayTag("Item.Consumable"));
	}
}

void AMeleeCombatSystemCharacter::ChangeWeapon()
{
	InventoryComponent->ChangeWeapon();
}


bool AMeleeCombatSystemCharacter::CanPerformDodge() const
{
	if (StateManager == nullptr)
		return false;

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Disabled")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dodging")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.GeneralActionState")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));


	return !StateManager->IsCurrentStateEqualToAny(States) && !GetCharacterMovement()->IsFalling();
}

bool AMeleeCombatSystemCharacter::CanPerformAttack() const
{
	if (StateManager == nullptr || CombatComponent->GetMainWeapon() == nullptr || !CombatComponent->IsCombatEnabled())
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

bool AMeleeCombatSystemCharacter::CanPerformBlock() const
{
	if (StateManager == nullptr || CombatComponent->GetMainWeapon() == nullptr || !CombatComponent->IsCombatEnabled())
		return false;

	// If weapon cannot block
	if (!CombatComponent->GetMainWeapon()->CanBlock())
	{
		return false;
	}

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Dead"));
	States.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Disabled"));
	States.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Dodging"));
	States.AddTag(FGameplayTag::RequestGameplayTag("Character.State.GeneralActionState"));
	States.AddTag(FGameplayTag::RequestGameplayTag("Character.State.TogglingCombat"));

	return !StateManager->IsCurrentStateEqualToAny(States) && !GetCharacterMovement()->IsFalling() && bIsBlockPressed;
}

bool AMeleeCombatSystemCharacter::CanUseItem() const
{
	if (StateManager == nullptr)
		return false;

	FGameplayTagContainer States;
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Disabled")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Dodging")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.Attacking")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.GeneralActionState")));
	States.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));


	return !StateManager->IsCurrentStateEqualToAny(States) && !GetCharacterMovement()->IsFalling();
}


void AMeleeCombatSystemCharacter::ContinueAttack(FGameplayTag AttackType)
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->PerformNextAttack(AttackType);
	}
}

void AMeleeCombatSystemCharacter::ResetAttack()
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->ResetAttack();
	}
}

FRotator AMeleeCombatSystemCharacter::GetDesiredRotation() const
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

void AMeleeCombatSystemCharacter::ResetCombat()
{
	if (CombatComponent != nullptr)
	{
		CombatComponent->ResetCombat();
		if (CanPerformBlock())
		{
			CombatComponent->SetBlockingState(true);
		}
		else
		{
			CombatComponent->SetBlockingState(false);
		}
	}
}

void AMeleeCombatSystemCharacter::EnableRagdoll()
{
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);

	CameraBoom->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepWorldTransform, PelvisBoneName);
	CameraBoom->bDoCollisionTest = false;

	GetMesh()->SetCollisionProfileName(TEXT("ragdoll"));
	GetMesh()->SetAllBodiesBelowSimulatePhysics(PelvisBoneName, true);
	GetMesh()->SetAllBodiesBelowPhysicsBlendWeight(PelvisBoneName, 1.0f);
}

void AMeleeCombatSystemCharacter::ApplyHitReactionPhysics(float InitialSpeed)
{
	FVector Speed = GetActorForwardVector() * -InitialSpeed;
	if (!bHitFront)
	{
		Speed = GetActorForwardVector() * InitialSpeed;
	}
	GetMesh()->SetPhysicsLinearVelocity(Speed);
}

void AMeleeCombatSystemCharacter::Die(bool bRandomIndex)
{
	UE_LOG(LogTemp, Display, TEXT("%s died"), *GetActorNameOrLabel());

	StateManager->SetCurrentState(FGameplayTag::RequestGameplayTag(FName("Character.State.Dead")));

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
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

	//// Destroy character
	//FTimerHandle CharacterDestroyTimer;
	//GetWorld()->GetTimerManager().SetTimer(CharacterDestroyTimer,
	//	[this]() {Destroy(); },
	//	DeathDestroyTimer, false);

	//// Destroy main weapon
	//if (CombatComponent->GetMainWeapon() != nullptr)
	//{
	//	CombatComponent->GetMainWeapon()->SetActorEnableCollision(false);
	//	CombatComponent->GetMainWeapon()->SimulateWeaponPhysics();
	//	FTimerHandle WeaponDestroyTimer;
	//	GetWorld()->GetTimerManager().SetTimer(WeaponDestroyTimer,
	//		[this]() {CombatComponent->GetMainWeapon()->Destroy(); },
	//		WeaponDeathDestroyTimer, false);
	//}
}

void AMeleeCombatSystemCharacter::RegenStamina()
{
	PerformKnockDown();
}

void AMeleeCombatSystemCharacter::LockOnTarget()
{
	if (TargetingComponent != nullptr)
	{
		TargetingComponent->ToggleLockOn();
	}
}

bool AMeleeCombatSystemCharacter::CanReceiveDamage() const
{
	if (StateManager != nullptr)
	{
		if (StateManager->GetCurrentState() != FGameplayTag::RequestGameplayTag("Character.State.Dead") &&
			!bIFramesEnabled)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool AMeleeCombatSystemCharacter::CanPerformAnimation() const
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

void AMeleeCombatSystemCharacter::SetInvincibleFrames(bool bEnableIFrames)
{
	bIFramesEnabled = bEnableIFrames;
}

void AMeleeCombatSystemCharacter::SetHyperarmorFrames(bool bEnableHFrames)
{
	bHFramesEnabled = bEnableHFrames;
}


void AMeleeCombatSystemCharacter::UpdateHealthPotionAmount(int Amount)
{
	ACombatPlayerController* CombatController = Cast<ACombatPlayerController>(GetController());
	if (CombatController != nullptr)
	{
		CombatController->SetPotionAmount(Amount);
	}
}

FGameplayTag AMeleeCombatSystemCharacter::GetCurrentState() const
{
	if (StateManager != nullptr)
	{
		return StateManager->GetCurrentState();
	}
	return FGameplayTag::EmptyTag;
}

void AMeleeCombatSystemCharacter::SetCurrentState(FGameplayTag NewState)
{
	if (StateManager != nullptr)
	{
		StateManager->SetCurrentState(NewState);
	}
}

void AMeleeCombatSystemCharacter::ResetState()
{
	if (StateManager != nullptr)
	{
		StateManager->ResetState();
	}
}

FGameplayTag AMeleeCombatSystemCharacter::GetCurrentAction() const
{
	if (StateManager != nullptr)
	{
		return StateManager->GetCurrentAction();
	}
	return FGameplayTag::EmptyTag;
}

void AMeleeCombatSystemCharacter::SetCurrentAction(FGameplayTag NewAction)
{
	if (StateManager != nullptr)
	{
		StateManager->SetCurrentAction(NewAction);
	}
}

void AMeleeCombatSystemCharacter::ResetAction()
{
	if (StateManager != nullptr)
	{
		StateManager->ResetAction();
	}
}



void AMeleeCombatSystemCharacter::Move(const FInputActionValue& Value)
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

void AMeleeCombatSystemCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		if (TargetingComponent != nullptr && TargetingComponent->IsTargeting())
		{
			return;
		}
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMeleeCombatSystemCharacter::ToggleCombat()
{
	if (CanPerformAnimation())
	{
		PerformAction(FGameplayTag::RequestGameplayTag(FName("Character.Action.ToggleCombat")),
			FGameplayTag::RequestGameplayTag(FName("Character.State.TogglingCombat")));
	}
}

void AMeleeCombatSystemCharacter::Interact()
{
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_GameTraceChannel1));
	TArray<AActor*> ObjectsToIgnore;
	FHitResult Hit;
	bool bWasHit = UKismetSystemLibrary::SphereTraceSingleForObjects
	(
		this,
		GetActorLocation(),
		GetActorLocation(),
		100.0f,
		ObjectTypesArray,
		true,
		ObjectsToIgnore,
		EDrawDebugTrace::None,
		Hit,
		true
	);
	if (bWasHit)
	{
		AActor* HitActor = Hit.GetActor();
		IInteractable* InteractableActor = Cast<IInteractable>(HitActor);
		if (InteractableActor != nullptr)
		{
			InteractableActor->Interact(this);
		}
	}
}

void AMeleeCombatSystemCharacter::LightAttack()
{
	if (CombatComponent == nullptr) return;

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


void AMeleeCombatSystemCharacter::HeavyAndSprintAttack()
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

void AMeleeCombatSystemCharacter::ChargedAttack()
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

void AMeleeCombatSystemCharacter::Dodge()
{
	if (CanPerformDodge())
	{
		PerformAction(FGameplayTag::RequestGameplayTag(FName("Character.Action.Dodge")),
			FGameplayTag::RequestGameplayTag(FName("Character.State.Dodging")));

	}
}

void AMeleeCombatSystemCharacter::Jump()
{
	if (CanPerformAnimation())
	{
		Super::Jump();

		if (StatsComponent != nullptr)
		{
			StatsComponent->ModifyCurrentStatValue(EStats::Stamina, -GetActionStaminaCost(FGameplayTag::RequestGameplayTag("Character.Action.Jump")));
		}
	}
}

void AMeleeCombatSystemCharacter::ToggleWalk()
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

void AMeleeCombatSystemCharacter::Sprint()
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

void AMeleeCombatSystemCharacter::Sprinting()
{
	if (StatsComponent != nullptr)
	{
		// Stop running if stamina is too low
		if (StatsComponent->GetCurrentStatValue(EStats::Stamina) <= 10)
		{
			SetMovementSpeedMode(EMovementSpeedMode::Jogging);
		}
		// Remove stamina only if running
		if (GetCharacterMovement()->Velocity.Size() >= SprintSpeed-0.1f)
		{
			StatsComponent->ModifyCurrentStatValue(EStats::Stamina, -GetActionStaminaCost(FGameplayTag::RequestGameplayTag("Character.Action.Sprint")));
		}
	}
}

void AMeleeCombatSystemCharacter::StopSprint()
{
	SetMovementSpeedMode(EMovementSpeedMode::Jogging);
	GetWorld()->GetTimerManager().ClearTimer(ChargeAttackTimerHandle);
	bIsSprintAttack = false;
}


void AMeleeCombatSystemCharacter::SwitchOnBeginStates(FGameplayTag State)
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

void AMeleeCombatSystemCharacter::SwitchOnEndStates(FGameplayTag State)
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

void AMeleeCombatSystemCharacter::SwitchOnBeginAction(FGameplayTag Action)
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

	}
	else if (Action.GetTagName() == "Character.Action.Nothing")
	{

	}
}