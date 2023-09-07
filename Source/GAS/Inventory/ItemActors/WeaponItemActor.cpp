// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/GASPhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"

AWeaponItemActor::AWeaponItemActor()
{
	
}

const UWeaponStaticData* AWeaponItemActor::GetWeaponStaticData() const
{
	return ItemInstance ? Cast<UWeaponStaticData>(ItemInstance->GetItemStaticData()) : nullptr;
}

FVector AWeaponItemActor::GetMuzzleLocation() const
{
	return MeshComponent ? MeshComponent->GetSocketLocation(TEXT("Muzzle")) : GetActorLocation();
}

void AWeaponItemActor::PlayWeaponEffects(const FHitResult& InHitResult)
{
	if(HasAuthority())
	{
		MulticastPlayWeaponEffects(InHitResult);
	}
	else
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::MulticastPlayWeaponEffects_Implementation(const FHitResult& InHitResult)
{
	if(!Owner || Owner->GetLocalRole() != ROLE_AutonomousProxy)
	{
		PlayWeaponEffectsInternal(InHitResult);
	}
}

void AWeaponItemActor::PlayWeaponEffectsInternal(const FHitResult& InHitResult)
{
	if(InHitResult.PhysMaterial.Get())
	{
		UGASPhysicalMaterial* PhysicalMaterial = Cast<UGASPhysicalMaterial>( InHitResult.PhysMaterial.Get());

		if(PhysicalMaterial)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PhysicalMaterial->PointImpactSound, InHitResult.ImpactPoint, 1.f);

			UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, PhysicalMaterial->PointImpactVFX, InHitResult.ImpactPoint);
		}
	}

	if(const UWeaponStaticData* weaponData = GetWeaponStaticData())
	{
		UGameplayStatics::PlaySoundAtLocation(this, weaponData->AttackSound, GetActorLocation(), 1.f);
	}
}

void AWeaponItemActor::InitInternal()
{
	Super::InitInternal();
	
	if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
	{
		if(WeaponStaticData->SkeletalMesh)
		{
			UE_LOG(LogTemp, Warning, TEXT("SkeletalMeshName : %s"), *WeaponStaticData->SkeletalMesh->GetName());

			
			USkeletalMeshComponent* SkeletalComp =
				NewObject<USkeletalMeshComponent>(
					this,
					USkeletalMeshComponent::StaticClass(),
					TEXT("MeshComponent"));

			if(SkeletalComp)
			{
				SkeletalComp->RegisterComponent();
				SkeletalComp->SetSkeletalMesh(WeaponStaticData->SkeletalMesh);
				SkeletalComp->AttachToComponent(GetRootComponent(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = SkeletalComp;
			}
		}
		else if(WeaponStaticData->StaticMesh)
		{
			UStaticMeshComponent* StaticComp =
				NewObject<UStaticMeshComponent>(
					this,
					UStaticMeshComponent::StaticClass(),
					TEXT("MeshComponent"));

			if(StaticComp)
			{
				StaticComp->RegisterComponent();
				StaticComp->SetStaticMesh(WeaponStaticData->StaticMesh);
				StaticComp->AttachToComponent(GetRootComponent(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale);

				MeshComponent = StaticComp;
			}
		}
	}
}
