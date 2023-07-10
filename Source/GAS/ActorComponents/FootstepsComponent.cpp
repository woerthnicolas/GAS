// Fill out your copyright notice in the Description page of Project Settings.

#include "FootstepsComponent.h"
#include "GASCharacter.h"
#include "PhysicalMaterials/GASPhysicalMaterial.h"
#include "Kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

static TAutoConsoleVariable<int32> CVarShowFootsteps(
	TEXT("ShowDebugFootsteps"),
	0,
	TEXT("Draws debug info about footsteps")
	TEXT(" 0: off/n")
	TEXT(" 1:on/n"),
	ECVF_Cheat
);

// Sets default values for this component's properties
UFootstepsComponent::UFootstepsComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

void UFootstepsComponent::HandleFootstep(EFoot Foot)
{
	if (AGASCharacter* Character = Cast<AGASCharacter>(GetOwner()))
	{
		const int32 DebugShowFootSteps = CVarShowFootsteps.GetValueOnAnyThread();

		if (USkeletalMeshComponent* Mesh = Character->GetMesh())
		{
			FHitResult HitResult;
			const FVector SocketLocation = Mesh->GetSocketLocation(
				Foot == EFoot::Left ? LeftFootSocketName : RightFootSocketName);
			const FVector Location = SocketLocation + FVector::UpVector * 20;

			FCollisionQueryParams QueryParams;
			QueryParams.bReturnPhysicalMaterial = true;
			QueryParams.AddIgnoredActor(Character);

			if (GetWorld()->LineTraceSingleByChannel(HitResult, Location, Location + FVector::UpVector * -50.0f,
			                                         ECollisionChannel::ECC_WorldStatic, QueryParams))
			{
				if (HitResult.bBlockingHit)
				{
					if (HitResult.PhysMaterial.Get())
					{
						UGASPhysicalMaterial* PhysicalMaterial = Cast<UGASPhysicalMaterial>(
							HitResult.PhysMaterial.Get());
						if (PhysicalMaterial)
						{
							UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->FootstepSound, Location, 1.f);
						}

						if (DebugShowFootSteps > 0)
						{
							DrawDebugString(GetWorld(), Location, GetNameSafe(PhysicalMaterial), nullptr, FColor::White,
							                4.f);
						}
					}
					if (DebugShowFootSteps > 0)
					{
						DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
					}
				}
				else
				{
					if (DebugShowFootSteps > 0)
					{
						DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4,
						              0,
						              1);
					}
				}
			}
			else
			{
				if (DebugShowFootSteps > 0)
				{
					DrawDebugLine(GetWorld(), Location, Location + FVector::UpVector * -50.f, FColor::Red, false, 4,
								  0,
								  1);
					DrawDebugSphere(GetWorld(), Location, 16, 16, FColor::Red, false, 4.f);
				}
			}
		}
	}
}
