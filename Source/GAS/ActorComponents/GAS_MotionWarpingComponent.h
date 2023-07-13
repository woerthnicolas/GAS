// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MotionWarpingComponent.h"
#include "ActionGameTypes.h"
#include "GAS_MotionWarpingComponent.generated.h"

struct FMotionWarpingTargetByLocationAndRotation;
/**
 * 
 */
UCLASS()
class GAS_API UGAS_MotionWarpingComponent : public UMotionWarpingComponent
{
	GENERATED_BODY()
public:
	UGAS_MotionWarpingComponent(const FObjectInitializer& ObjectInitializer);

	void SendWarpPointsToClients();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSyncWarpPoints(const TArray<FMotionWarpingTargetByLocationAndRotation>& Targets);
};
