// Copyright Epic Games, Inc. All Rights Reserved.

#include "gioco_testCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"


#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"


//////////////////////////////////////////////////////////////////////////
// Agioco_testCharacter

Agioco_testCharacter::Agioco_testCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	RunningSpeed = 650.f;
	SprintingSpeed = 950.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	bShiftKeyDown = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

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

	bRoll = false;

	bFurtive = false;
	
	FurtiveSpeed = 300.f;
<<<<<<< HEAD


=======
<<<<<<< HEAD


	
=======
>>>>>>> ce80e7657457a34a1c27a67a825aa5062be519b0

>>>>>>> e1cf9c0841a9e5b60133853419923c7970d7982f

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input
void Agioco_testCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bShiftKeyDown && !(GetMovementComponent()->IsFalling()) && (MovementStatus != EMovementStatus::EMS_Furtive))
	{
		SetMovementStatus(EMovementStatus::EMS_Sprinting);
	}
	else if((!bFurtive) || (GetMovementComponent()->IsFalling()))
	{
		bFurtive = false;
		SetMovementStatus(EMovementStatus::EMS_Normal);
	}
	else
	{
		
	}


}

void Agioco_testCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

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


}



void Agioco_testCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
<<<<<<< HEAD

=======
<<<<<<< HEAD
=======

>>>>>>> ce80e7657457a34a1c27a67a825aa5062be519b0
>>>>>>> e1cf9c0841a9e5b60133853419923c7970d7982f
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
	if (!bRoll && !(GetMovementComponent()->IsFalling()))
	{
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
	bShiftKeyDown = true;
}
void Agioco_testCharacter::ShiftKeyUp()
{
	bShiftKeyDown = false;
}
void Agioco_testCharacter::SetMovementStatus(EMovementStatus Status)
{
	MovementStatus = Status;
	if (MovementStatus == EMovementStatus::EMS_Sprinting)
	{
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	}
	else if(!bFurtive)
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
	if(bFurtive)
		SetMovementStatus(EMovementStatus::EMS_Furtive);
	else
		SetMovementStatus(EMovementStatus::EMS_Normal);
}