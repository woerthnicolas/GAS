// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Abilities//GameplayAbility.h"
#include "AbilitySystemInterface.h"
#include "ActionGameTypes.h"
#include "InputActionValue.h"
#include "GASCharacter.generated.h"

class UFootstepsComponent;
class UCharacterDataAsset;
struct FCharacterData;
class UGASAbilitySystemComponentBase;
class UGASAttributeSetBase;

class UGameplayEffect;
class UGameplayAbility;

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

	virtual void PostInitializeComponents() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

protected:
	
	bool ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect, FGameplayEffectContextHandle InEffectContext);

	virtual void PawnClientRestart() override;

public:
	virtual void Landed(const FHitResult& Hit) override;

protected:
	
	void GiveAbilities();
	void ApplyStartupEffects();

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UPROPERTY(EditDefaultsOnly)
	UGASAbilitySystemComponentBase* AbilitySystemComponent;
	
	UPROPERTY(Transient)
	UGASAttributeSetBase* AttributeSet;
	
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

protected:
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

	void OnMoveForwardAction(const FInputActionValue& Value);

	void OnMoveSideAction(const FInputActionValue& Value);

	void OnTurnAction(const FInputActionValue& Value);

	void OnLookUpAction(const FInputActionValue& Value);

	void OnJumpActionStarted(const FInputActionValue& Value);

	void OnJumpActionEnded(const FInputActionValue& Value);

	// Gameplay Events
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag JumpEventTag;

	//Gameplay Tags

	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer InAirTags;
};

