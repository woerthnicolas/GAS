// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GASCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"

static TAutoConsoleVariable<int32> CVarShowTraversal(
	TEXT("ShowDebugTraversal"),
	0,
	TEXT("Draws debug info about traversal")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);


void UGASCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	HandleMovementDirection();

	if (UAbilitySystemComponent* AbilityComponent =
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner()))
	{
		AbilityComponent->RegisterGameplayTagEvent(
			                FGameplayTag::RequestGameplayTag(TEXT("Movement.Enforced.Strafe"), EGameplayTagEventType::NewOrRemoved))
		                .AddUObject(this, &UGASCharacterMovementComponent::OnEnforcedStrafeTagChanged);
	}
}

bool UGASCharacterMovementComponent::TryTraversal(UAbilitySystemComponent* ASC)
{
	for (const auto AbilityClass : TraversalAbilitiesOrdered)
	{
		if (ASC->TryActivateAbilityByClass(AbilityClass, true))
		{
			FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromClass(AbilityClass);

			if (Spec && Spec->IsActive())
			{
				return true;
			}
		}
	}

	return false;
}

EMovementDirectionType UGASCharacterMovementComponent::GetMovementDirectionType() const
{
	return MovementDirectionType;
}

void UGASCharacterMovementComponent::SetMovementDirectionType(EMovementDirectionType InMovementDirectionType)
{
	MovementDirectionType = InMovementDirectionType;

	HandleMovementDirection();
}

void UGASCharacterMovementComponent::OnEnforcedStrafeTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (NewCount)
	{
		SetMovementDirectionType(EMovementDirectionType::Strafe);
	}
	else
	{
		SetMovementDirectionType(EMovementDirectionType::OrientToMovement);
	}
}

void UGASCharacterMovementComponent::HandleMovementDirection()
{
	switch (MovementDirectionType)
	{
	case EMovementDirectionType::None:
		break;
	case EMovementDirectionType::OrientToMovement:
		break;
	case EMovementDirectionType::Strafe:
		bUseControllerDesiredRotation = true;
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = false;
		break;
	default:
		bUseControllerDesiredRotation = false;
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = true;;
	}
}
