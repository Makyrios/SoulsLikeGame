// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CombatType.h"

#include "CoreMinimal.h"
#include "BaseEquippable.h"
#include "ECharacterAction.h"
#include "GameplayTags.h"
#include "ECollisionPart.h"
#include "AttackDamageType.h"
#include "BaseWeapon.generated.h"


/**
 * 
 */
UCLASS()
class MELEECOMBATSYSTEM_API ABaseWeapon : public ABaseEquippable
{
	GENERATED_BODY()

public:
	ABaseWeapon();

	virtual void Tick(float DeltaTime) override;

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	void SimulateWeaponPhysics();

	UFUNCTION(BlueprintCallable)
	virtual void ActivateCollision(ECollisionPart CollisionPart);
	UFUNCTION(BlueprintCallable)
	virtual void DeactivateCollision(ECollisionPart CollisionPart);

	UFUNCTION(BlueprintCallable)
	virtual void ToggleCombat(bool bEnableCombat);


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Initialization")
	ECombatType CombatType_;


protected:
	virtual void BeginPlay() override;

	virtual void OnHitEvent(FHitResult HitActor);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCollisionComponent* MainWeaponCollisionComponent;

	class UCombatComponent* CombatComponent;
	class UStateManager* StateManager;

public:
	/** Returns name of socket for a holding weapon hand */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetHandSocketName() const { return HandSocketName; }
	
	FORCEINLINE UAnimMontage* GetEnterCombatAnimMontage() const { return EnterCombatAnimMontage; }
	FORCEINLINE UAnimMontage* GetExitCombatAnimMontage() const { return ExitCombatAnimMontage; }

	FORCEINLINE ECombatType GetCombatType() const { return CombatType_; }
	FORCEINLINE void SetCombatType(ECombatType Type) { CombatType_ = Type; }

	float GetActionStatCost() const;

	float GetDamage();

	virtual TArray<UAnimMontage*> GetActionMontages(FGameplayTag Action) const;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool CanBlock() const { return bCanBlock; }

private:
	UPROPERTY(EditAnywhere, Category = "Socket")
	FName HandSocketName = "WeaponSocket";

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* EnterCombatAnimMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	UAnimMontage* ExitCombatAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> LightAttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> HeavyAttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> ChargedAttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> FallingAttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> SprintAttackMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Animations")
	TArray<UAnimMontage*> BlockReactionMontages;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	bool bCanBlock = true;

	UPROPERTY(EditAnywhere, Category = "Stats")
	float BaseDamage = 10;

	UPROPERTY(EditAnywhere, Category = "Stats")
	TMap<FGameplayTag, float> ActionStatCost;

	UPROPERTY(EditAnywhere, Category = "Stats")
	TMap<FGameplayTag, float> ActionDamageMultiplier;

	UPROPERTY(EditAnywhere, Category = "Initialization")
	TArray<UStaticMesh*> WeaponMeshes;

	UPROPERTY(EditDefaultsOnly, Category = "Initialization")
	TSubclassOf<UDamageType> DamageTypeClass;

};
