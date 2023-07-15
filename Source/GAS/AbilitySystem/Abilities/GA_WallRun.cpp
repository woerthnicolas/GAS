// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_WallRun.h"

#include "AbilitySystemComponent.h"
#include "GASCharacter.h"
#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_WallRun::UGA_WallRun()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGA_WallRun::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	if (const ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor))
	{
		UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

		CapsuleComponent->OnComponentHit.AddDynamic(this, &ThisClass::OnCapsuleComponentHit);
	}
}

void UGA_WallRun::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	if (ActorInfo)
	{
		if (const ACharacter* CharacterAvatar = Cast<ACharacter>(ActorInfo->AvatarActor))
		{
			UCapsuleComponent* CapsuleComponent = CharacterAvatar->GetCapsuleComponent();

			CapsuleComponent->OnComponentHit.RemoveDynamic(this, &ThisClass::OnCapsuleComponentHit);
		}
	}

	Super::OnRemoveAbility(ActorInfo, Spec);
}

bool UGA_WallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const AGASCharacter* CharacterAvatar = GetCharacterFromActorInfo();

	return CharacterAvatar && !CharacterAvatar->GetCharacterMovement()->IsMovingOnGround();
}

void UGA_WallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	WallRunTickTask = UAbilityTask_TickWallRun::CreateWallRunTask(
		this,
		Cast<ACharacter>(GetAvatarActorFromActorInfo()),
		Cast<UCharacterMovementComponent>(ActorInfo->MovementComponent),
		WallRun_TraceObjectTypes
	);

	WallRunTickTask->OnFinished.AddDynamic(this, &ThisClass::K2_EndAbility);
	WallRunTickTask->OnWallRunWallSideDetermined.AddDynamic(this, &ThisClass::OnWallSideDetermened);

	WallRunTickTask->ReadyForActivation();
}

void UGA_WallRun::OnCapsuleComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->TryActivateAbility(GetCurrentAbilitySpec()->Handle);
	}
}

void UGA_WallRun::OnWallSideDetermened(bool bLeftSide)
{
	AGASCharacter* CharacterAvatar = GetCharacterFromActorInfo();
	if(!CharacterAvatar) return;
	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if(!AbilitySystemComponent) return;
	FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();

	if(bLeftSide)
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunLeftSideEffectClass, EffectContextHandle);
	}
	else
	{
		CharacterAvatar->ApplyGameplayEffectToSelf(WallRunRightSideEffectClass, EffectContextHandle);
	}
}

void UGA_WallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsValid(WallRunTickTask))
	{
		WallRunTickTask->EndTask();
	}

	UAbilitySystemComponent* AbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	if(AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunLeftSideEffectClass, AbilitySystemComponent);
		AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(WallRunRightSideEffectClass, AbilitySystemComponent);
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
