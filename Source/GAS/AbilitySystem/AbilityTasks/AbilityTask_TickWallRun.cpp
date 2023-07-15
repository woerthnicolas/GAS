// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_TickWallRun.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"

void UAbilityTask_TickWallRun::Activate()
{
	Super::Activate();

	FHitResult OneWallHit;

	//const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if(!FindRunnableWall(OneWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();

		return;
	}

	OnWallRunWallSideDetermined.Broadcast(IsWallOnTheLeft(OneWallHit));

	CharacterOwner->Landed(OneWallHit);

	CharacterOwner->SetActorLocation(OneWallHit.ImpactPoint + OneWallHit.ImpactNormal * 60.f);

	CharacterMovement->SetMovementMode(MOVE_Flying);
}

UAbilityTask_TickWallRun* UAbilityTask_TickWallRun::CreateWallRunTask(UGameplayAbility* OwningAbility, ACharacter* InCharacter, UCharacterMovementComponent* InCharacterMovement, TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes)
{
	UAbilityTask_TickWallRun* WallRunTask = NewAbilityTask<UAbilityTask_TickWallRun>(OwningAbility);

	WallRunTask->CharacterMovement = InCharacterMovement;
	WallRunTask->CharacterOwner = InCharacter;
	WallRunTask->bTickingTask = true;
	WallRunTask->WallRun_TraceObjectTypes = TraceObjectTypes;
	
	return WallRunTask;
}

bool UAbilityTask_TickWallRun::FindRunnableWall(FHitResult& OnWallHit)
{
	const FVector CharacterLocation = CharacterOwner->GetActorLocation();

	const FVector RightVector = CharacterOwner->GetActorRightVector();
	const FVector ForwardVector = CharacterOwner->GetActorForwardVector();

	const float TraceLength = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius() + 30.f;

	TArray<AActor*> ActorsToIgnore {CharacterOwner};

	static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugTraversal"));

	const bool bShowTraversal = CVar->GetInt() > 0;

	EDrawDebugTrace::Type DebugDrawType = bShowTraversal ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + ForwardVector * TraceLength,
		WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if(FVector::DotProduct(OnWallHit.ImpactNormal, RightVector) > 0.3f)
		{
			return true;
		}
	}

	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), CharacterLocation, CharacterLocation + RightVector * TraceLength,
		WallRun_TraceObjectTypes, true, ActorsToIgnore, DebugDrawType, OnWallHit, true))
	{
		if(FVector::DotProduct(OnWallHit.ImpactNormal, -RightVector) > 0.3f)
		{
			return true;
		}
	}

	return false;
}

bool UAbilityTask_TickWallRun::IsWallOnTheLeft(const FHitResult& InWallHit) const
{
	return FVector::DotProduct(CharacterOwner->GetActorRightVector(), InWallHit.ImpactNormal) > 0.f;
}

void UAbilityTask_TickWallRun::TickTask(float DeltaTime)
{
	Super::TickTask(DeltaTime);

	FHitResult OnWallHit;
	//const FVector CurrentAcceleration = CharacterMovement->GetCurrentAcceleration();

	if(!FindRunnableWall(OnWallHit))
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}

		EndTask();

		return;
	}

	const FRotator DirectionRotator = IsWallOnTheLeft(OnWallHit) ? FRotator(0, -90, 0) : FRotator(0, 90, 0);

	const FVector WallRunDirection = DirectionRotator.RotateVector(OnWallHit.ImpactNormal);

	CharacterMovement->Velocity = WallRunDirection * 700.f;
	// CharacterMovement->Velocity.Z = CharacterMovement->GetGravityZ() * DeltaTime;
	CharacterMovement->Velocity.Z = 0;
	CharacterOwner->SetActorRotation(WallRunDirection.Rotation());

	CharacterMovement->SetPlaneConstraintEnabled(true);
	CharacterMovement->SetPlaneConstraintOrigin(OnWallHit.ImpactPoint);
	CharacterMovement->SetPlaneConstraintNormal(OnWallHit.ImpactNormal);
}

void UAbilityTask_TickWallRun::OnDestroy(bool bInOwnerFinished)
{
	CharacterMovement->SetPlaneConstraintEnabled(false);
	CharacterMovement->SetMovementMode(MOVE_Falling);
	
	Super::OnDestroy(bInOwnerFinished);
}
