// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonEnemyAnimInstance.h"
#include "SkeletonEnemy.h"

void USkeletonEnemyAnimInstance::SetSkeletonEnemyPawn()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			SkeletonEnemy = Cast<ASkeletonEnemy>(Pawn);
		}
	}
}


void USkeletonEnemyAnimInstance::NativeInitializeAnimation()
{
	SetSkeletonEnemyPawn();
}

void USkeletonEnemyAnimInstance::UpdateCharacterAnimationProperties()
{
	SetSkeletonEnemyPawn();
	if (Pawn)
	{
		FVector Speed = Pawn->GetVelocity();
		FVector HorizontalSpeed = FVector(Speed.X, Speed.Y, 0);
		//Magnitude
		CharacterSpeed = HorizontalSpeed.Size();
	}
}
