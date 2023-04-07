// Copyright Epic Games, Inc. All Rights Reserved.

#include "gioco_testCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Weapon.h"


//////////////////////////////////////////////////////////////////////////
// Agioco_testCharacter

Agioco_testCharacter::Agioco_testCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	RunningSpeed = 500.f;
	SprintingSpeed = 1150.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.01f;

	MovementStatus = EMovementStatus::EMS_Normal;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	
	FurtiveSpeed = 300.f;




	//Initialize Enums
	StaminaStatus = EStaminaStatus::ESS_Normal;

	StaminaDrainRate = 25.f;
	MinSprintStamina = 50.f;
	MaxStamina = 150.f;
	Stamina = 120.f;

	bJump = false;
	bFurtive = false;
	bRoll = false;
	bShiftKeyDown = false;
	bAttacking = false;
	bSlowTime = false;
	bRMBDown = false;


	

	
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
void Agioco_testCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	float DeltaStamina = StaminaDrainRate * DeltaTime;

		switch (StaminaStatus)
		{
		case EStaminaStatus::ESS_Normal:
			if (bShiftKeyDown && ((GetVelocity().Size()) > 0.1f) && !(GetMovementComponent()->IsFalling()) && !bFurtive)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				if (Stamina - DeltaStamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= DeltaStamina;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
			}
			else if (bRoll)
			{
				if (Stamina <= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
					Stamina -= 0.8f;
				}
				else
				{
					Stamina -= 0.8f;
				}
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MaxStamina)
				{
					Stamina = MaxStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
			}
			break;
		case EStaminaStatus::ESS_BelowMinimum:
			if (bShiftKeyDown && ((GetVelocity().Size()) > 0.1f) && !(GetMovementComponent()->IsFalling()) && !bFurtive)
			{
				SetMovementStatus(EMovementStatus::EMS_Sprinting);
				if (Stamina - DeltaStamina < 0.f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
				}
				else
				{
					Stamina -= DeltaStamina;
				}
			}
			else if (bRoll)
			{
				if (Stamina < 0.f)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Exhausted);
					Stamina = 0;
				}
				else
				{
					Stamina -= 0.8f;
				}
			}
			else // Shift key up
			{
				if (Stamina + DeltaStamina >= MinSprintStamina)
				{
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
					Stamina += DeltaStamina;
				}
				else
				{
					Stamina += DeltaStamina;
				}
			}

			break;

		case EStaminaStatus::ESS_Exhausted:
			if (bShiftKeyDown && ((GetVelocity().Size()) > 0.1f) && !(GetMovementComponent()->IsFalling()) && !bFurtive)
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
				Stamina = 0.f;
			}
			else if (bRoll)
			{
				Stamina = 0.f;
			}
			else // Shift key up
			{
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				Stamina += DeltaStamina;
			}

			break;
		case EStaminaStatus::ESS_ExhaustedRecovering:
			if (bFurtive)
			{
				SetMovementStatus(EMovementStatus::EMS_Furtive);
			}
			else
			{
				SetMovementStatus(EMovementStatus::EMS_Normal);
			}
			if (Stamina + DeltaStamina >= MinSprintStamina)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else if (bRoll)
			{
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
				Stamina += DeltaStamina;
			}
			else
			{
				Stamina += DeltaStamina;
			}
			break;
		default:
			;

		}
}

void Agioco_testCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &Agioco_testCharacter::Jump_TestCharacter);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &Agioco_testCharacter::StopJump_TestCharacter);

	PlayerInputComponent->BindAxis("MoveForward", this, &Agioco_testCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &Agioco_testCharacter::MoveRight);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &Agioco_testCharacter::ShiftKeyDown);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &Agioco_testCharacter::ShiftKeyUp);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &Agioco_testCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &Agioco_testCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &Agioco_testCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &Agioco_testCharacter::TouchStopped);


	// Roll key bindings
	PlayerInputComponent->BindAction("roll", IE_Pressed, this, &Agioco_testCharacter::Roll_Start);
	
	PlayerInputComponent->BindAction("furtivity", IE_Pressed, this, &Agioco_testCharacter::Furtivity_Mode);

	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &Agioco_testCharacter::LMBDown);
	PlayerInputComponent->BindAction("LMB", IE_Released, this, &Agioco_testCharacter::LMBUp);

	PlayerInputComponent->BindAction("stop_time", IE_Pressed, this, &Agioco_testCharacter::SlowMotion);

	PlayerInputComponent->BindAction("RMB", IE_Pressed, this, &Agioco_testCharacter::RMBDown);
	PlayerInputComponent->BindAction("RMB", IE_Released, this, &Agioco_testCharacter::RMBUp);


}

void Agioco_testCharacter::Jump_TestCharacter()
{
	if (!bFurtive && !bRoll)
	{
		bJump = true;
		SetMovementStatus(EMovementStatus::EMS_Jumping);
		Super::Jump();
		float DelayTime = 0.1;
		GetWorldTimerManager().SetTimer(DelayTimer, this, &Agioco_testCharacter::StopJump_TestCharacter, DelayTime);
	}
}

void Agioco_testCharacter::StopJump_TestCharacter()
{
	if (!bFurtive)
	{
		Super::StopJumping();
		SetMovementStatus(EMovementStatus::EMS_Normal);
		bJump = false;
		
	}
}



void Agioco_testCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{

		Jump();
}

void Agioco_testCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();

}

void Agioco_testCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void Agioco_testCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void Agioco_testCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void Agioco_testCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}



void Agioco_testCharacter::Roll_Start()
{
	if (!bRoll && !(GetMovementComponent()->IsFalling()) && (StaminaStatus != EStaminaStatus::ESS_Exhausted) && (StaminaStatus != EStaminaStatus::ESS_ExhaustedRecovering))
	{
		bAttacking = false;
		bRoll = true;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && GeneralMontage)
		{
		AnimInstance->Montage_Play(GeneralMontage, 1.2f);
		AnimInstance->Montage_JumpToSection(FName("roll"), GeneralMontage);
		}
	}
	
}




void Agioco_testCharacter::Stop_Roll()
{
	bRoll = false;

}


void Agioco_testCharacter::ShiftKeyDown()
{
	if (!bFurtive && !GetMovementComponent()->IsFalling())
	{
		bShiftKeyDown = true;
		
	}
}



void Agioco_testCharacter::ShiftKeyUp()
{
	if (!bFurtive)
	{
		bShiftKeyDown = false;
		SetMovementStatus(EMovementStatus::EMS_Normal);
	}
}




void Agioco_testCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if(MovementStatus == EMovementStatus::EMS_Normal)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = FurtiveSpeed;
	}

}


void Agioco_testCharacter::Furtivity_Mode()
{
	bFurtive = !bFurtive;
	if (bFurtive)
	{
		SetMovementStatus(EMovementStatus::EMS_Furtive);
		bShiftKeyDown = false;
	}
	else
	{
		SetMovementStatus(EMovementStatus::EMS_Normal);
	}
}


void Agioco_testCharacter::LMBUp()
{
	if (bRoll) return;
	bLMBDown = false;
}

void Agioco_testCharacter::LMBDown()
{
	if (bRoll) return;
	bLMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;


	if (ActiveOverlappingItem)
	{
		AWeapon* Weapon = Cast<AWeapon>(ActiveOverlappingItem);
		if (Weapon)
		{
			Weapon->Equip(this);
			SetActiveOverlappingItem(nullptr);
		}
	}
	else if (EquippedWeapon)
	{
		Attack();
	}

}

void Agioco_testCharacter::SetEquippedWeapon(AWeapon* WeaponToSet)
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}
	EquippedWeapon = WeaponToSet;
}

void Agioco_testCharacter::Attack()
{
	if (!bAttacking && MovementStatus != EMovementStatus::EMS_Dead)
	{
		bAttacking = true;
		//SetInterpToEnemy(true);

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

		if (AnimInstance && GeneralMontage)
		{
			if (bLMBDown)
			{
				AnimInstance->Montage_Play(GeneralMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("attack_1"), GeneralMontage);
			}
			else
			{
				AnimInstance->Montage_Play(GeneralMontage, 1.2f);
				AnimInstance->Montage_JumpToSection(FName("attack_2"), GeneralMontage);
			}
		}


	}
}

void Agioco_testCharacter::AttackEnd()
{
	bAttacking = false;
	//SetInterpToEnemy(false);
	if (bLMBDown)
	{
		Attack();
	}
}

void Agioco_testCharacter::PlaySwingSound()
{
	if (EquippedWeapon->SwingSound)
	{
		UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}


void Agioco_testCharacter::SlowMotion()
{
	bSlowTime = !bSlowTime;
	if (bSlowTime)
	{
		CustomTimeDilation = 5;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);

	}
	else
	{
		CustomTimeDilation = 1;
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
}

void Agioco_testCharacter::RMBUp()
{
	if (bRoll) return;
	bRMBDown = false;
}

void Agioco_testCharacter::RMBDown()
{
	if (bRoll) return;
	bRMBDown = true;

	if (MovementStatus == EMovementStatus::EMS_Dead) return;

	if (EquippedWeapon)
	{
		Attack();
	}
	else return;
}
