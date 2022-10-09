// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Knight.generated.h"


UENUM(BlueprintType)
enum class EKnightLocomotionStatus : uint8
{
	E_WALK UMETA(DisplayName = "Walk"),
	E_RUN UMETA(DisplayName = "Running"),
	E_DEATH UMETA(DisplayName = "Death")
};

UENUM(BlueprintType)
enum class EKnightStaminaStatus : uint8
{
	ES_NORMAL UMETA(DisplayName = "Normal"),
	ES_UNDERMINIMUM UMETA(DisplayName = "Underminimum"),
	ES_ZERO UMETA(DisplayName = "Zero"),
	ES_RECOVER UMETA(DisplayName = "Recover")
};

UCLASS()
class THEMIST_API AKnight : public ACharacter
{
	GENERATED_BODY()

public:
	AKnight();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		EKnightLocomotionStatus KnightLocomotionStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		EKnightStaminaStatus KnightStaminaStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* CameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoxComponent* SwordCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UParticleSystem* HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CharacterTurnRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Stamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Mana;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float WalkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float RunSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float StaminaUsage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float MinimumRunStamina;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bRunKeyPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bRollKeyPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bCombat;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bRoll;

	bool bLMB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SwordDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		class UAnimMontage* KnightAnimMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSubclassOf<UDamageType> TypeOfDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		AController* SwordInstigator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FTimerHandle RollDelay;

	bool bMoveForwardBackward;

	bool bMoveRightLeft;

	void MoveCharacterForwardBackward(float Value);

	void MoveCharacterRightOrLeft(float Value);

	void LookCharacterUpOrDownAtRate(float Rate);

	void TurnCharacterRightOrLeftAtRate(float Rate);

	class USpringArmComponent* GetCameraArm() const;

	class UCameraComponent* GetCamera() const;

	void DecreaseHealth(float Amount);

	void IncreaseHealth(float Amount);

	void DecreaseMana(float Amount);

	void IncreaseMana(float Amount);

	void Die();

	void SetKnightLocomotionStatus(EKnightLocomotionStatus Status);

	void SetKnightStaminaStatus(EKnightStaminaStatus Status);

	void RunKeyPressed();

	void RunKeyNotPressed();

	void RollKeyPressed();

	void RollKeyNotPressed();

	void LeftMouseButtonActive();

	void LeftMouseButtonNotActive();

	void StartCombatMontage();

	void StartRollMontage();

	void StopRolling();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
		void StopCombatMontage();

	UFUNCTION(BlueprintCallable)
		void StopRollMontage();

	UFUNCTION()
		void SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
		void DisableWeaponCollision();

	void SetInstigator(AController* Insti);

	UFUNCTION(BlueprintCallable)
		void EndAtDeath();

	void PlayAnimationAndJumpToSection(FName Section, float Speed);
	void StopAnimation();
};
