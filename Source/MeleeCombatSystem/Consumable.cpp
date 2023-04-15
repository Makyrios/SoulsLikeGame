// Fill out your copyright notice in the Description page of Project Settings.

#include "Consumable.h"
#include "StateControl.h"
#include "Combat.h"


void AConsumable::PerformItemAction()
{
	IStateControl* OwnerState = Cast<IStateControl>(GetOwner());
	if (OwnerState != nullptr)
	{
		if (CurrentNumberOfUses > 0)
		{
			OwnerState->PerformCustomAction(FGameplayTag::RequestGameplayTag("Character.Action.UseItem"),
				FGameplayTag::RequestGameplayTag("Character.State.GeneralActionState"), UseItemMontage);

		}
	}
}

void AConsumable::ConsumeItem()
{
	CurrentNumberOfUses--;
}

void AConsumable::BeginPlay()
{
	Super::BeginPlay();

	CurrentNumberOfUses = MaxNumberOfUses;
}

void AConsumable::OnEquipped()
{
	Super::OnEquipped();

	AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
}


