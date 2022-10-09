// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "SkeletonEnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THEMIST_API USkeletonEnemyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	void SetSkeletonEnemyPawn();

	UFUNCTION(BlueprintCallable)
		void UpdateCharacterAnimationProperties();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class APawn* Pawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class ASkeletonEnemy* SkeletonEnemy;
	
};
