// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "KnightAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class THEMIST_API UKnightAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CharacterSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class APawn* Pawn;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class AKnight* KnightCharacte;

	UFUNCTION(BlueprintCallable)
		void UpdateCharacterAnimationProperties();
};
