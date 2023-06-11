// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Combat.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "StateControl.h"
#include "EMovementSpeedMode.h"
#include "GameplayTagContainer.h"
#include "GameplayTags.h"
#include "EDamageType.h"
#include "Components/TimelineComponent.h"
#include "MeleeCombatSystemCharacter.generated.h"

class ABaseEquippable;
class ABaseWeapon;
class UCombatComponent;
class UStateManager;
class UStatsComponent;
class UTargetingComponent;
class UAIPerceptionStimuliSourceComponent;
class UEquipmentComponent;
class UInventoryComponent;

UCLASS(config = Game)
class AMeleeCombatSystemCharacter : public ACharacter, public ICombat, public IStateControl, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	/** ToggleCombat Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ToggleCombatAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* InteractAction;

	/** Light Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LightAttackAction;

	/** Heavy Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* HeavyAttackAction;

	/** Sprint Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ChargedAttackAction;

	/** Dodge Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* DodgeAction;

	/** Toggle Walk Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ToggleWalkAction;

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	/** Target Lock On Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LockOnTargetAction;

	/** Block Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* BlockAction;

	/** Use Item Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* UseItemAction;

	/** Use Change Weapon Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ChangeWeaponAction;

public:
	AMeleeCombatSystemCharacter();

	virtual void Tick(float DeltaTime) override;

	// ICombat implementations
	virtual void ContinueAttack(FGameplayTag AttackType) override;
	virtual void ResetAttack() override;
	virtual FRotator GetDesiredRotation() const override;
	virtual void ResetCombat() override;
	virtual bool CanReceiveDamage() const override;
	virtual bool CanPerformAnimation() const override;
	virtual void SetInvincibleFrames(bool bEnableIFrames) override;
	virtual void SetHyperarmorFrames(bool bEnableHFrames) override;
	virtual void UpdateHealthPotionAmount(int Amount) override;


	// IStateControl implementations
	virtual FGameplayTag GetCurrentState() const override;
	virtual void SetCurrentState(FGameplayTag NewState) override;
	virtual void ResetState() override;

	virtual FGameplayTag GetCurrentAction() const override;
	virtual void SetCurrentAction(FGameplayTag NewAction) override;
	virtual void ResetAction() override;
	virtual TArray<UAnimMontage*> GetMontageByTag(FGameplayTag Action) const override;
	virtual float PerformAction(FGameplayTag Action, FGameplayTag State, bool bRandomIndex = false, int MontageIndex = 0) override;
	virtual float PerformCustomAction(FGameplayTag ActionTag, FGameplayTag StateTag, UAnimMontage* InMontage, float MontagePlayRate = 1.0f, bool bAutoReset = false) override;
	virtual float PerformAttack(FGameplayTag AttackTag, int MontageIndex, bool bRandomIndex) override;

	void PerformBlock();

	bool CanPerformDodge() const;

	bool CanPerformAttack() const;

	bool CanPerformBlock() const;

	bool CanUseItem() const;

	void EnableRagdoll();

	void ApplyHitReactionPhysics(float InitialSpeed);

	void Die(bool bRandomIndex = true);

	void RegenStamina();

	void LockOnTarget();


protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for toggling combat input */
	void ToggleCombat();

	/** Called for interact input */
	void Interact();

	/** Called on released attack input */
	void LightAttack();

	/** Called on attack with shift input */
	void HeavyAndSprintAttack();

	/** Called on attack with shift input */
	void ChargedAttack();

	/** Called for dodge input */
	void Dodge();

	/** Called for jump input */
	void Jump() override;

	/** Called for toggle walk input */
	void ToggleWalk();

	/** Called for sprint input */
	void Sprint();
	void Sprinting();
	void StopSprint();

	/** Called for block input */
	void StartBlock();
	void StopBlock();

	/** Called for use item input */
	void PerformItemAction();

	/** Called for change weapon input */
	void ChangeWeapon();


protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	/*UFUNCTION()
	void TakePointDamage
	(
		AActor* DamagedActor,
		float Damage,
		class AController* InstigatedBy,
		FVector HitLocation,
		class UPrimitiveComponent* FHitComponent,
		FName BoneName,
		FVector ShotFromDirection,
		const class UDamageType* DamageType,
		AActor* DamageCauser
	);*/

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void FindIfBehind(AActor* DamageCauser);

	void ApplyImpactEffects(AActor* DamagedActor);

	UFUNCTION()
	void CheckDeath(EStats Type, float Value);

	void ApplyHitReaction(EDamageType DamageType);

	bool WasHitBlocked() const;

	void PerformHitStun();

	void PerformKnockDown();


public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }


	virtual EMovementSpeedMode GetMovementSpeedMode() const override { return MovementSpeedMode; }
	void SetMovementSpeedMode(EMovementSpeedMode NewMode);

	bool CanReceiveHitReaction() const;

	float GetActionStaminaCost(FGameplayTag Action) const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStateManager* StateManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStatsComponent* StatsComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UTargetingComponent* TargetingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UEquipmentComponent* EquipmentComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UInventoryComponent* InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UAIPerceptionStimuliSourceComponent* AIPerceptionSourceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UTimelineComponent* AttackTimeline;
	
	UPROPERTY(EditAnywhere, Category = "Movement")
	TMap<FGameplayTag, float> ActionsStaminaCost;

	FOnTimelineFloat AttackUpdateFunction;

public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	UCurveFloat* AttackTimelineFloatCurve;

	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer OwnedGameplayTags;

private:
	EMovementSpeedMode MovementSpeedMode = EMovementSpeedMode::Jogging;

	FTimerHandle ChargeAttackTimerHandle;

	bool bIsSprintAttack;
	bool bHitFront;
	bool bIFramesEnabled;
	bool bIsBlockPressed;
	bool bHFramesEnabled;

	UPROPERTY(EditAnywhere, Category = "Physics")
	float HitReactionInitialSpeed = 2000;

	UPROPERTY(EditAnywhere, Category = "Physics")
	FName PelvisBoneName;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* GuardianImpactSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
	UParticleSystem* GuardianImpactParticles;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* HitReactionFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* HitReactionBackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* KnockDownMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> DeathMontages;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float WalkSpeed = 300;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float JogSpeed = 500;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float SprintSpeed = 700;


	void SwitchOnBeginStates(FGameplayTag State);
	void SwitchOnEndStates(FGameplayTag State);

	void SwitchOnBeginAction(FGameplayTag Action);
	
	void RotateToTarget();
	void StopRotatingToTarget();

	UFUNCTION()
	void UpdateRotationToTarget(float Value);

	// Inherited via IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
};

