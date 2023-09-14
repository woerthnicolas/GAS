// Fill out your copyright notice in the Description page of Project Settings.


#include "GASBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actors/Projectile.h"
#include "Kismet/KismetSystemLibrary.h"

static TAutoConsoleVariable<int32> CVarShowRadialDamage(
	TEXT("ShowRadialDamage"),
	0,
	TEXT("Draws debug info about radial damage")
	TEXT(" 0: off/n")
	TEXT(" 1: on/n"),
	ECVF_Cheat
);

const UItemStaticData* UGASBlueprintFunctionLibrary::GetItemStaticData(TSubclassOf<UItemStaticData> ItemDataClass)
{
	if (IsValid(ItemDataClass))
	{
		return GetDefault<UItemStaticData>(ItemDataClass);
	}

	return nullptr;
}

void UGASBlueprintFunctionLibrary::ApplyRadialDamage(UObject* WorldContextObject, AActor* DamageCauser,
                                                     FVector Location, float Radius, float DamageAmount,
                                                     TArray<TSubclassOf<UGameplayEffect>> DamageEffects,
                                                     const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
                                                     ETraceTypeQuery TraceType)
{
	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore = {DamageCauser};

	UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, Location, Radius, ObjectTypes, nullptr,
	                                          ActorsToIgnore, OutActors);

	const bool bDebug = static_cast<bool>(CVarShowRadialDamage.GetValueOnAnyThread());

	for (AActor* Actor : OutActors)
	{
		FHitResult HitResult;

		if (UKismetSystemLibrary::LineTraceSingle(WorldContextObject, Location, Actor->GetActorLocation(), TraceType,
		                                          true, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true))
		{
			AActor* Target = HitResult.GetActor();
			if (Target == Actor)
			{
				bool bWasApplied = true;

				if (UAbilitySystemComponent* AbilitySystemComponent =
					UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target))
				{
					FGameplayEffectContextHandle EffectContextHandle = AbilitySystemComponent->MakeEffectContext();
					EffectContextHandle.AddInstigator(DamageCauser, DamageCauser);

					for (auto Effect : DamageEffects)
					{
						FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
							Effect, 1, EffectContextHandle);

						if (SpecHandle.IsValid())
						{
							UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(
								SpecHandle, FGameplayTag::RequestGameplayTag(TEXT("Attribute.Health")),
								-DamageAmount);

							FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->
								ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
							if (ActiveGEHandle.WasSuccessfullyApplied())
							{
								bWasApplied = true;
							}
						}
					}
				}

				if (bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(),
					              bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16,
					                bWasApplied ? FColor::Green : FColor::Red, false, 4.f, 0, 1);
					DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr,
					                FColor::White, 0, false, 1.f);
				}
			}
			else
			{
				if (bDebug)
				{
					DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red,
					              false, 4.f, 0, 1);
					DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f,
					                0, 1);
					DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(Target), nullptr,
					                FColor::White, 0, false, 1.f);
				}
			}
		}
		else
		{
			if (bDebug)
			{
				DrawDebugLine(WorldContextObject->GetWorld(), Location, Actor->GetActorLocation(), FColor::Red, false,
				              4.f, 0, 1);
				DrawDebugSphere(WorldContextObject->GetWorld(), HitResult.Location, 16, 16, FColor::Red, false, 4.f, 0,
				                1);
				DrawDebugString(WorldContextObject->GetWorld(), HitResult.Location, *GetNameSafe(HitResult.GetActor()),
				                nullptr, FColor::White, 0, false, 1.f);
			}
		}
	}

	if (bDebug)
	{
		DrawDebugSphere(WorldContextObject->GetWorld(), Location, Radius, 16, FColor::White, false, 4.f, 0, 1.f);
	}
}

AProjectile* UGASBlueprintFunctionLibrary::LaunchProjectile(UObject* WorldContextObject,
                                                            TSubclassOf<UProjectileStaticData> ProjectileStaticData,
                                                            FTransform Transform, AActor* Owner, APawn* Instigator)
{
	UWorld* World = WorldContextObject ? WorldContextObject->GetWorld() : nullptr;

	if (World && World->IsServer())
	{
		if (AProjectile* Projectile = World->SpawnActorDeferred<AProjectile>(
			AProjectile::StaticClass(), Transform, Owner, Instigator, ESpawnActorCollisionHandlingMethod::AlwaysSpawn))
		{
			Projectile->ProjectileDataClass = ProjectileStaticData;
			Projectile->FinishSpawning(Transform);

			return Projectile;
		}
	}

	return nullptr;
}
