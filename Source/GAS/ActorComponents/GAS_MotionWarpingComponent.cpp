// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorComponents/GAS_MotionWarpingComponent.h"

UGAS_MotionWarpingComponent::UGAS_MotionWarpingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}
void UGAS_MotionWarpingComponent::SendWarpPointsToClients()
{
	if (GetOwnerRole() == ROLE_Authority)
	{
		TArray<FMotionWarpingTargetByLocationAndRotation> WarpTargets;

		for (auto WarpTarget : WarpTargetMap)
		{
			FMotionWarpingTargetByLocationAndRotation MotionWarpingTarget(WarpTarget.Key, WarpTarget.Value.GetLocation(), WarpTarget.Value.GetRotation());

			WarpTargets.Add(MotionWarpingTarget);
		}

		MulticastSyncWarpPoints(WarpTargets);
	}
}

void UGAS_MotionWarpingComponent::MulticastSyncWarpPoints_Implementation(const TArray<FMotionWarpingTargetByLocationAndRotation>& Targets)
{
	for (const FMotionWarpingTargetByLocationAndRotation& Target : Targets)
	{
		AddOrUpdateWarpTargetFromLocationAndRotation(Target.Name, Target.Location, FRotator(Target.Rotation));
	}
}