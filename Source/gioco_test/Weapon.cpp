// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AWeapon::AWeapon()
{	
	//Create skeletalmesh component for the weapon
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//Attach to the principal component
	SkeletalMesh->SetupAttachment(GetRootComponent());

	//Collision box for the sword
	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->SetupAttachment(GetRootComponent());

	// This body Will have no representation in the physics engine.It cannot be used for spatial queries(raycasts, sweeps, overlaps) or simulation(rigid body, constraints).This setting gives the best performance possible, especially for moving objects.
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//Set collision type to worldDynamic(object theat can move with some animation)
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	//Ignore collision event from other things that are not pawn(our character). The weapon will overlap with the character
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);




}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}


