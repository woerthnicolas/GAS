// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "ActionGameTypes.h"
#include "GASBlueprintFunctionLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

static TAutoConsoleVariable<int32> CVarShowProjectiles(
	TEXT("ShowDebugProjectiles"),
	0,
	TEXT("Draws debug info about projectiles")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//SetReplicateMovement(true);
	bReplicates = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	ProjectileMovementComponent->Velocity = FVector::ZeroVector;
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &ThisClass::AProjectile::OnProjectileStop);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	StaticMeshComponent->SetupAttachment(GetRootComponent());
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Projectile"));
	StaticMeshComponent->bReceivesDecals = false;
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData && ProjectileMovementComponent)
	{
		if(ProjectileStaticData->StaticMesh)
		{
			StaticMeshComponent->SetStaticMesh(ProjectileStaticData->StaticMesh);
		}

		ProjectileMovementComponent->bInitialVelocityInLocalSpace = false;
		ProjectileMovementComponent->InitialSpeed = ProjectileStaticData->InitialSpeed;
		ProjectileMovementComponent->MaxSpeed = ProjectileStaticData->MaxSpeed;
		ProjectileMovementComponent->bRotationFollowsVelocity = true;
		ProjectileMovementComponent->bShouldBounce = false;
		ProjectileMovementComponent->Bounciness = 0.f;
		ProjectileMovementComponent->ProjectileGravityScale = ProjectileStaticData->GravityMultiplayer;

		ProjectileMovementComponent->Velocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
	}

	const int32 DebugShowProjectile = CVarShowProjectiles.GetValueOnAnyThread();
	if(DebugShowProjectile)
	{
		DebugDrawPath();
	}
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();

	if(ProjectileStaticData)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ProjectileStaticData->OnStopSFX, GetActorLocation(), 1.f);

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ProjectileStaticData->OnStopVFX, GetActorLocation());
	}
	
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const UProjectileStaticData* AProjectile::GetProjectileStaticData() const
{
	if(IsValid(ProjectileDataClass))
	{
		return GetDefault<UProjectileStaticData>(ProjectileDataClass);
	}

	return nullptr;
}

void AProjectile::DebugDrawPath() const
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();
	if(ProjectileStaticData)
	{
		FPredictProjectilePathParams PredictProjectilePathParams;
		PredictProjectilePathParams.StartLocation = GetActorLocation();
		PredictProjectilePathParams.LaunchVelocity = ProjectileStaticData->InitialSpeed * GetActorForwardVector();
		PredictProjectilePathParams.TraceChannel = ECC_Visibility;
		PredictProjectilePathParams.bTraceComplex = true;
		PredictProjectilePathParams.bTraceWithCollision = true;
		PredictProjectilePathParams.DrawDebugType = EDrawDebugTrace::ForDuration;
		PredictProjectilePathParams.DrawDebugTime = 3.f;
		PredictProjectilePathParams.OverrideGravityZ = ProjectileStaticData->GravityMultiplayer;

		FPredictProjectilePathResult PredictProjectilePathResult;
		if(UGameplayStatics::PredictProjectilePath(this, PredictProjectilePathParams, PredictProjectilePathResult))
		{
			DrawDebugSphere(GetWorld(), PredictProjectilePathResult.HitResult.Location, 50, 10, FColor::Red);
		}
	}
}

void AProjectile::OnProjectileStop(const FHitResult& ImpactResult)
{
	const UProjectileStaticData* ProjectileStaticData = GetProjectileStaticData();
	if(ProjectileStaticData)
	{
		UGASBlueprintFunctionLibrary::ApplyRadialDamage(this, GetOwner(), GetActorLocation(),
		ProjectileStaticData->DamageRadius,
		ProjectileStaticData->BaseDamage,
		ProjectileStaticData->Effects,
		ProjectileStaticData->RadialDamageQueryTypes,
		ProjectileStaticData->RadialDamageTraceType);
	}

	Destroy();
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AProjectile, ProjectileDataClass);
}
