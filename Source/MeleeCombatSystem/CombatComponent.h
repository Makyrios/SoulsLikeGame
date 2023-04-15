// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECharacterAction.h"
#include "ECharacterState.h"
#include "GameplayTagsManager.h"
#include "GameplayTagContainer.h"
#include "CombatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCombatEnabled, bool, bIsAttackEnabled);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShieldSet, bool, bIsShieldEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBlockingSet, bool, bIsBlockingSet);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, ECombatType, CombatType);


class ABaseWeapon;
class ABaseShield;
class ACharacter;
class IStateControl;
class UStatsComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MELEECOMBATSYSTEM_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnShieldSet OnShieldEquippedSet;

	UPROPERTY(BlueprintAssignable)
	FOnCombatEnabled OnCombatEnabled;

	UPROPERTY(BlueprintAssignable)
	FOnBlockingSet OnBlockingSet;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponEquipped OnWeaponEquipped;


	// Sets default values for this component's properties
	UCombatComponent();

	/** Called for toggling combat input */
	void ToggleCombat();

	float PerformAttack(FGameplayTag AttackType, int AttackIndex, bool bRandomIndex);

	void PerformNextAttack(FGameplayTag AttackType);

	void ResetAttack();

	void ResetCombat();

	//float PerformAction(FGameplayTag Action, FGameplayTag State, int MontageIndex = 0);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE ABaseWeapon* GetMainWeapon() const { return MainWeapon; }
	FORCEINLINE void SetMainWeapon(ABaseWeapon* Value) { MainWeapon = Value; }
	void ChangeMainWeapon(ABaseWeapon* Value);


	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsBlocking() const { return bIsBlocking; }

	FORCEINLINE void SetIsBlocking(bool Value) { bIsBlocking = Value; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsCombatEnabled() const { return bIsCombatEnabled; }
	UFUNCTION(BlueprintCallable)
	void SetIsCombatEnabled(bool Value);

	void SetBlockingState(bool bEnableBlock);

	/*FORCEINLINE bool IsAttackSaved() const { return bIsAttackSaved; }
	FORCEINLINE void SetIsAttackSaved(bool Value) { bIsAttackSaved = Value; }*/


	FORCEINLINE int GetAttackCount() const { return AttackCount; }


	UFUNCTION(BlueprintCallable)
	FORCEINLINE FGameplayTag GetSavedAttack() const { return SavedAttack; }

	FORCEINLINE bool IsShieldEquipped() const { return bIsShieldEquipped; }
	void SetShieldEquipped(bool Value);


private:
	FGameplayTag SavedAttack;

	ACharacter* OwnerCharacter;
	ABaseWeapon* MainWeapon;
	IStateControl* OwnerStateControl;
	UStatsComponent* OwnerStatsComponent;

	bool bIsAttackSaved;
	bool bIsCombatEnabled;
	bool bIsBlocking;
	bool bIsShieldEquipped;

	int AttackCount;

};
