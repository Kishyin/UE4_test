// Fill out your copyright notice in the Description page of Project Settings.


#include "MainAnimInstance.h"
#include "gioco_test/gioco_testCharacter.h"


void UMainAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr) // controlla quale pawn è in controllo delle animazioni
	{
		Pawn = TryGetPawnOwner();
		if (Pawn)
		{
			Character = Cast<Agioco_testCharacter>(Pawn);
			Character->SetMainAnimInstance(this);
		}
	}

}

