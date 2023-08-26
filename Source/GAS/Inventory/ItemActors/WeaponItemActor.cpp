// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/ItemActors/WeaponItemActor.h"

#include "Inventory/InventoryItemInstance.h"

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

void AWeaponItemActor::InitInternal()
{
	Super::InitInternal();
	
	if(const UWeaponStaticData* WeaponStaticData = GetWeaponStaticData())
	{
		if(WeaponStaticData->SkeletalMesh)
		{
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
