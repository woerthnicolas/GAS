// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Abilities//GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "InputActionValue.h"
#include "ActorComponents/GAS_MotionWarpingComponent.h"
#include "GASCharacter.generated.h"

class UInventoryComponent;
class UFootstepsComponent;
class UCharacterDataAsset;
struct FCharacterData;
class UGASAbilitySystemComponentBase;
class UGASAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

class UGAS_MotionWarpingComponent;
class UGASCharacterMovementComponent;
class UInventoryComponent;

UCLASS(config=Game)
class AGASCharacter : public ACharacter, public IAbilitySystemInterface
{
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AGASCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void PostLoad() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual void PawnClientRestart() override;

public:
	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

protected:
	
	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(EditDefaultsOnly)
	UGASAbilitySystemComponentBase* AbilitySystemComponent;
	
	UPROPERTY(Transient)
	UGASAttributeSetBase* AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "MotionWarp")
	UGAS_MotionWarpingComponent* ActionGameMotionWarpingComponent;

	UGASCharacterMovementComponent* AGCharacterMovementComponent;
	
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

public:
	UFUNCTION(BlueprintCallable)
	FCharacterData GetCharacterData() const;

	UFUNCTION(BlueprintCallable)
	void SetCharacterData(const FCharacterData& InCharacterData);

	UFootstepsComponent* GetFootstepsComponent() const;

	UGAS_MotionWarpingComponent* GetMotionWarpingComponent() const;

	UInventoryComponent* GetInventoryComponent() const;

	void OnMaxMovementSpeedChanged(const FOnAttributeChangeData& Data);

protected:
	UPROPERTY(ReplicatedUsing=OnRep_CharacterData)
	FCharacterData CharacterData;

	UFUNCTION()
	void OnRep_CharacterData();

	virtual void InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication = false);

	UPROPERTY(EditDefaultsOnly)
	UCharacterDataAsset* CharacterDataAsset;

	UPROPERTY(BlueprintReadOnly)
	class UFootstepsComponent* FootstepsComponent;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveForwardInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* MoveSideInputAction;

	UPROPERTY(EditDefaultsOnly)
	class UInputAction* TurnInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* LookUpInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* JumpInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* CrouchInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* SprintInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* DropItemInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* EquipNextInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* UnequipInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* AttackInputAction;
	
	UPROPERTY(EditDefaultsOnly)
	class UInputAction* AimInputAction;

	void OnMoveForwardAction(const FInputActionValue& Value);

	void OnMoveSideAction(const FInputActionValue& Value);

	void OnTurnAction(const FInputActionValue& Value);

	void OnLookUpAction(const FInputActionValue& Value);

	void OnJumpActionStarted(const FInputActionValue& Value);

	void OnJumpActionEnded(const FInputActionValue& Value);

	void OnCrouchActionStarted(const FInputActionValue& Value);

	void OnCrouchActionEnded(const FInputActionValue& Value);

	void OnSprintActionStarted(const FInputActionValue& Value);

	void OnSprintActionEnded(const FInputActionValue& Value);
	
	void OnDropItemTriggered(const FInputActionValue& Value);
	
	void OnEquipNextTriggered(const FInputActionValue& Value);
	
	void OnUnequipTriggered(const FInputActionValue& Value);

	void OnAttackActionStarted(const FInputActionValue& Value);

	void OnAttackActionEnded(const FInputActionValue& Value);

	void OnAimActionStarted(const FInputActionValue& Value);

	void OnAimActionEnded(const FInputActionValue& Value);

	// Gameplay Events
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackStartedEventTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AttackEndedEventTag;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimStartedEventTag;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag AimEndedEventTag;

	// Gameplay Tags

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer CrouchTags;
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer SprintTags;

	// Gameplay Effects
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> CrouchStateEffect;

	// Delegates
	FDelegateHandle MaxMovementSpeedChangedDelegateHandle;

	UPROPERTY(EditAnywhere, Replicated)
	UInventoryComponent* InventoryComponent = nullptr;
};

