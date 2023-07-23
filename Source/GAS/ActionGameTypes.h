﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionGameTypes.generated.h"

class AItemActor;
class UGameplayAbility;
class UGameplayEffect;
class UAnimMontage;
class UNiagaraSystem;

USTRUCT(BlueprintType)
struct FCharacterData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> Effects;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
	class UCharacterAnimDataAsset* CharacterAnimDataAsset;
};

USTRUCT(BlueprintType)
struct FCharacterAnimationData
{
	GENERATED_USTRUCT_BODY();

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* MovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* IdleAnimationAsset = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UBlendSpace* CrouchMovementBlendspace = nullptr;

	UPROPERTY(EditDefaultsOnly)
	class UAnimSequenceBase* CrouchIdleAnimationAsset = nullptr;
};

UENUM(BlueprintType)
enum class EFoot : uint8
{
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

USTRUCT(BlueprintType)
struct FMotionWarpingTargetByLocationAndRotation
{
	GENERATED_USTRUCT_BODY();

	FMotionWarpingTargetByLocationAndRotation()
	{

	}

	FMotionWarpingTargetByLocationAndRotation(FName InName, FVector InLocation, FQuat InRotation)
		: Name(InName)
		, Location(InLocation)
		, Rotation(InRotation)
	{

	}

	UPROPERTY()
	FName Name;

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FQuat Rotation;
};

UCLASS(BlueprintType, Blueprintable)
class UItemStaticData : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AItemActor> ItemActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName AttachmentSocket = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bCanBeEquipped = false;

	UPROPERTY(EditDefaultsOnly)
	FCharacterAnimationData CharacterAnimationData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities;
};

UCLASS(BlueprintType, Blueprintable)
class UWeaponStaticData : public UItemStaticData
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* StaticMesh;
};
UENUM(BlueprintType)
enum class EItemState : uint8
{
	None  UMETA(DisplayName = "None"),
	Equipped UMETA(DisplayName = "Equipped"),
	Dropped UMETA(DisplayName = "Dropped"),
};
