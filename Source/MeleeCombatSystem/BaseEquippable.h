// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <GameplayTags.h>
#include "BaseEquippable.generated.h"

UCLASS()
class MELEECOMBATSYSTEM_API ABaseEquippable : public AActor, public IGameplayTagAssetInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseEquippable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void OnEquipped();
	virtual void OnUnequipped();

	bool IsEquipped() const;
	void SetIsEquipped(bool Value);

	UFUNCTION(BlueprintCallable)
	virtual void AttachActor(FName SocketName);

	UFUNCTION(BlueprintCallable)
	virtual void DetachActor();

	virtual void PerformItemAction();


public:
	FORCEINLINE UPrimitiveComponent* GetStaticMesh() const { return ItemStaticMesh; }
	FORCEINLINE UPrimitiveComponent* GetSkeletalMesh() const { return ItemSkeletalMesh; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE FName GetAttachSocketName() const { return AttachSocketName; }


protected:
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* ItemStaticMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USkeletalMeshComponent* ItemSkeletalMesh;

	UPROPERTY(EditAnywhere, Category = "Socket")
	FName AttachSocketName;

	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer OwnedGameplayTags;

	bool bIsEquipped;


	// Inherited via IGameplayTagAssetInterface
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

};
