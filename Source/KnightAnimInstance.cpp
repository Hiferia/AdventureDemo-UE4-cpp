// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightAnimInstance.h"
#include "Knight.h"

void UKnightAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	else
	{
		KnightCharacte = Cast<AKnight>(Pawn);
	}
}

void UKnightAnimInstance::UpdateCharacterAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}
	else
	{
		FVector Speed = Pawn->GetVelocity();
		FVector HorizontalSpeed = FVector(Speed.X, Speed.Y, 0);

		CharacterSpeed = HorizontalSpeed.Size();

		if (KnightCharacte == nullptr)
		{
			KnightCharacte = Cast<AKnight>(Pawn);
		}
	}
}
