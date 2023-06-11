// Fill out your copyright notice in the Description page of Project Settings.

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
#include "Targetable.h"
#include "Components/TimelineComponent.h"
#include "GenericTeamAgentInterface.h"
#include "MasterAI.generated.h"

class ABaseEquippable;
class ABaseWeapon;
class UCombatComponent;
class UStateManager;
class UStatsComponent;
class UTargetingComponent;
class UBehaviorTree;
class UEquipmentComponent;

UCLASS()
class MELEECOMBATSYSTEM_API AMasterAI : public ACharacter, public ICombat, public IStateControl, public ITargetable
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMasterAI();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
	TArray<class ATargetPoint*> PatrolPoints;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	// Inherited via ICombat
	virtual void ContinueAttack(FGameplayTag AttackType) override;
	virtual void ResetAttack() override;
	virtual FRotator GetDesiredRotation() const override;
	virtual void ResetCombat() override;
	virtual bool CanReceiveDamage() const override;
	virtual void SetInvincibleFrames(bool bEnableIFrames) override;
	virtual void SetHyperarmorFrames(bool bEnableHFrames) override;
	virtual void UpdateHealthPotionAmount(int Amount) override;

	// Inherited via IStateControl
	virtual bool CanPerformAnimation() const override;
	virtual FGameplayTag GetCurrentState() const override;
	virtual void SetCurrentState(FGameplayTag NewState) override;
	virtual void ResetState() override;
	virtual FGameplayTag GetCurrentAction() const override;
	virtual void SetCurrentAction(FGameplayTag NewAction) override;
	virtual void ResetAction() override;
	virtual TArray<UAnimMontage*> GetMontageByTag(FGameplayTag Action) const override;
	virtual EMovementSpeedMode GetMovementSpeedMode() const override { return MovementSpeedMode; }

	virtual float PerformAction(FGameplayTag Action, FGameplayTag State, bool bRandomIndex = false, int MontageIndex = 0) override;
	virtual float PerformCustomAction(FGameplayTag ActionTag, FGameplayTag StateTag, UAnimMontage* InMontage, float MontagePlayRate = 1.0f, bool bAutoReset = false) override;
	virtual float PerformAttack(FGameplayTag ActionTag, int MontageIndex, bool bRandomIndex) override;


	// Inherited via ITargetable
	virtual bool CanBeTargeted() override;
	virtual void OnTargeted(bool bIsTargeted) override;


	bool CanPerformAttack() const;

	void EnableRagdoll();

	void ApplyHitReactionPhysics(float InitialSpeed);

	UFUNCTION(BlueprintCallable)
	void PerformSpawn(bool bRandomIndex = true);

	void Die(bool bRandomIndex = true);

	void OnTargetSet(AActor* Target);

protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called on released attack input */
	void LightAttack();

	/** Called on attack with shift input */
	void HeavyAndSprintAttack();

	/** Called on attack with shift input */
	void ChargedAttack();

	/** Called for toggling combat input */
	bool ToggleCombat();

	/** Called for jump input */
	void Jump() override;

	/** Called for toggle walk input */
	void ToggleWalk();

	/** Called for sprint input */
	void Sprint();
	void StopSprint();

protected:

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	void FindIfBehind(AActor* DamageCauser);

	void ApplyImpactEffects(AActor* DamagedActor);

	UFUNCTION()
	void CheckDeath(EStats Type, float Value);

	void ApplyHitReaction(EDamageType DamageType);

	void PerformHitStun();

	void PerformKnockDown();


public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }

	UFUNCTION(BlueprintCallable)
	void SetMovementSpeedMode(EMovementSpeedMode NewMode);

	bool CanReceiveHitReaction() const;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		UStateManager* StateManager;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		UStatsComponent* StatsComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
		UEquipmentComponent* EquipmentComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UTimelineComponent* AttackTimeline;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Initialization")
	UBehaviorTree* BehaviorTree;

	FOnTimelineFloat AttackUpdateFunction;

	virtual void SwitchOnBeginStates(FGameplayTag State);
	virtual void SwitchOnEndStates(FGameplayTag State);

	virtual void SwitchOnBeginAction(FGameplayTag Action);

public:
	UPROPERTY(EditAnywhere, Category = "Combat")
	UCurveFloat* AttackTimelineFloatCurve;

private:
	AActor* TargetActor;

	EMovementSpeedMode MovementSpeedMode = EMovementSpeedMode::Jogging;

	FTimerHandle ChargeAttackTimerHandle;

	bool bIsSprintAttack;
	bool bHitFront;
	bool bIFramesEnabled;
	bool bHFramesEnabled;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	bool bSpawnImmediately = true;

	UPROPERTY(EditAnywhere, Category = "Physics")
		float HitReactionInitialSpeed = 2000;

	UPROPERTY(EditAnywhere, Category = "Physics")
		FName PelvisBoneName;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
		USoundBase* GuardianImpactSoundCue;

	UPROPERTY(EditDefaultsOnly, Category = "Particles")
		UParticleSystem* GuardianImpactParticles;

	UPROPERTY(EditAnywhere, Category = "Animations")
		TArray<UAnimMontage*> SpawnMontages;

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

	UPROPERTY(EditAnywhere, Category = "Character")
		float WeaponDeathDestroyTimer = 10;

	UPROPERTY(EditAnywhere, Category = "Character")
		float DeathDestroyTimer = 10;

	UPROPERTY(EditDefaultsOnly, Category = "HUD")
		class UWidgetComponent* LockOnWidgetComponent;


	void RotateToTarget();
	void StopRotatingToTarget();

	UFUNCTION()
	void UpdateRotationToTarget(float Value);

	void ToggleActor(bool bHide);
};
