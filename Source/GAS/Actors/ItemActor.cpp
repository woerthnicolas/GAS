// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "ActorComponents/InventoryComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/ActorChannel.h"
#include "Inventory/InventoryItemInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AItemActor::AItemActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	AActor::SetReplicateMovement(true);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
}

void AItemActor::Init(UInventoryItemInstance* InInstance)
{
	ItemInstance = InInstance;
}

// Called when the game starts or when spawned
void AItemActor::BeginPlay()
{
	Super::BeginPlay();

	if(HasAuthority())
	{
		if(!IsValid(ItemInstance) && IsValid(ItemStaticDataClass))
		{
			ItemInstance = NewObject<UInventoryItemInstance>();
			ItemInstance->Init(ItemStaticDataClass);

			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SphereComponent->SetGenerateOverlapEvents(true);
		}
		InitInternal();
	}
}

void AItemActor::InitInternal()
{
	
}

// Called every frame
void AItemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AItemActor::OnEquipped()
{
	ItemState = EItemState::Equipped;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnUnequipped()
{
	ItemState = EItemState::None;

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SphereComponent->SetGenerateOverlapEvents(false);
}

void AItemActor::OnDropped()
{
	ItemState = EItemState::Dropped;
	
	GetRootComponent()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	
	if (AActor* ActorOwner = GetOwner())
	{
		const FVector Location = GetActorLocation();
		const FVector Forward = ActorOwner->GetActorForwardVector();

		constexpr float dropItemDist = 100.f;
		constexpr float dropItemTraceDist = 10000.f;
		const FVector TraceStart = Location + Forward * dropItemDist;
		const FVector TraceEnd = Location + -FVector::UpVector * dropItemTraceDist;

		TArray<AActor*> ActorsToIgnore = {GetOwner()};
		FHitResult TraceHit;

		static const auto CVar = IConsoleManager::Get().FindConsoleVariable(TEXT("ShowDebugInventory"));
		const bool ShowDebugInventory = CVar->GetInt() > 0;

		FVector TargetLocation = TraceEnd;

		EDrawDebugTrace::Type DrawDebugType = ShowDebugInventory ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

		if (UKismetSystemLibrary::LineTraceSingleByProfile(
				this, TraceStart, TraceEnd, TEXT("WorldStatic"), true,
				ActorsToIgnore, DrawDebugType, TraceHit, true)
		)
		{
			if (TraceHit.bBlockingHit)
			{
				TargetLocation = TraceHit.Location;
			}
		}

		SetActorLocation(TargetLocation);
		
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		SphereComponent->SetGenerateOverlapEvents(true);
	}
}

void AItemActor::OnRep_ItemState()
{
	switch (ItemState) {
		case EItemState::Equipped:
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			SphereComponent->SetGenerateOverlapEvents(false);
			break;
		default:
			SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SphereComponent->SetGenerateOverlapEvents(true);
			break;
	}
}


void AItemActor::OnRep_ItemInstance(UInventoryItemInstance* OldItemInstance)
{
	if(IsValid(ItemInstance) && !IsValid(OldItemInstance))
	{
		InitInternal();
	}
}

void AItemActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(HasAuthority())
	{
		FGameplayEventData EventPayload;
		EventPayload.Instigator = this;
		EventPayload.OptionalObject = ItemInstance;
		EventPayload.EventTag = UInventoryComponent::EquipItemActorTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OtherActor, UInventoryComponent::EquipItemActorTag, EventPayload);
	}
}

bool AItemActor::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	WroteSomething |= Channel->ReplicateSubobject(ItemInstance, *Bunch, *RepFlags);

	return WroteSomething;
}


void AItemActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AItemActor, ItemInstance);
	DOREPLIFETIME(AItemActor, ItemState);
}
