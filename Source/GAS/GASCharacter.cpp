// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/Attribute/GASAttributeSetBase.h"
#include "AbilitySystem/Components/GASAbilitySystemComponentBase.h"
#include "ActorComponents/FootstepsComponent.h"
#include "ActorComponents/GASCharacterMovementComponent.h"
#include "DataAssets/CharacterDataAsset.h"
#include "Net/UnrealNetwork.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputModule.h"

//////////////////////////////////////////////////////////////////////////
// AGASCharacter

AGASCharacter::AGASCharacter(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer.SetDefaultSubobjectClass<UGASCharacterMovementComponent>(CharacterMovementComponentName))
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	AbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	AttributeSet = CreateDefaultSubobject<UGASAttributeSetBase>(TEXT("DefaultAttributeSet"));

	FootstepsComponent = CreateDefaultSubobject<UFootstepsComponent>(TEXT("FootstepsComponent"));
}

void AGASCharacter::PostInitializeComponents()
{
	if (IsValid(CharacterDataAsset))
	{
		SetCharacterData(CharacterDataAsset->CharacterData);
	}
	Super::PostInitializeComponents();
}


UAbilitySystemComponent* AGASCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGASCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if(MoveForwardInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveForwardInputAction, ETriggerEvent::Triggered, this, &AGASCharacter::OnMoveForwardAction);
		}

		if(MoveSideInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveSideInputAction, ETriggerEvent::Triggered, this, &AGASCharacter::OnMoveSideAction);
		}
		
		if(TurnInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(TurnInputAction, ETriggerEvent::Triggered, this, &AGASCharacter::OnTurnAction);
		}

		if(LookUpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(LookUpInputAction, ETriggerEvent::Triggered, this, &AGASCharacter::OnLookUpAction);
		}
		
		if(JumpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &AGASCharacter::OnJumpActionStarted);
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &AGASCharacter::OnJumpActionEnded);
		}
	}
}

FCharacterData AGASCharacter::GetCharacterData() const
{
	return CharacterData;
}

void AGASCharacter::SetCharacterData(const FCharacterData& InCharacterData)
{
	CharacterData = InCharacterData;

	InitFromCharacterData(CharacterData);
}

UFootstepsComponent* AGASCharacter::GetFootstepsComponent() const
{
	return FootstepsComponent;
}

void AGASCharacter::OnRep_CharacterData()
{
	InitFromCharacterData(CharacterData, true);
}

void AGASCharacter::InitFromCharacterData(const FCharacterData& InCharacterData, bool bFromReplication)
{
}

void AGASCharacter::OnMoveForwardAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ((Controller != nullptr) && (Magnitude != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Magnitude);
	}
}

void AGASCharacter::OnMoveSideAction(const FInputActionValue& Value)
{
	const float Magnitude = Value.GetMagnitude();
	if ((Controller != nullptr) && (Magnitude != 0.0f))
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Magnitude);
	}
}

void AGASCharacter::OnTurnAction(const FInputActionValue& Value)
{
	AddControllerYawInput(Value.GetMagnitude() * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGASCharacter::OnLookUpAction(const FInputActionValue& Value)
{
	AddControllerPitchInput(Value.GetMagnitude() * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AGASCharacter::OnJumpActionStarted(const FInputActionValue& Value)
{
	Jump();
}

void AGASCharacter::OnJumpActionEnded(const FInputActionValue& Value)
{
	StopJumping();
}

bool AGASCharacter::ApplyGameplayEffectToSelf(TSubclassOf<UGameplayEffect> Effect,
                                              FGameplayEffectContextHandle InEffectContext)
{
	if (!Effect.Get()) return false;

	FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(Effect, 1, InEffectContext);
	if (SpecHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(
			*SpecHandle.Data.Get());

		return ActiveGEHandle.WasSuccessfullyApplied();
	}

	return false;
}

void AGASCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	
	if(APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if(UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();

			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGASCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (auto DefaultAbility : CharacterData.Abilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility));
		}
	}
}

void AGASCharacter::ApplyStartupEffects()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		for (auto CharacterEffect : CharacterData.Effects)
		{
			ApplyGameplayEffectToSelf(CharacterEffect, EffectContext);
		}
	}
}

void AGASCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	GiveAbilities();
	ApplyStartupEffects();
}

void AGASCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AGASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGASCharacter, CharacterData);
}
