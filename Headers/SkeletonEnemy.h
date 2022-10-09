// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SkeletonEnemy.generated.h"

UENUM(BlueprintType)
enum class ESkeletonStatus : uint8
{
	E_IDLE,
	E_RUN,
	E_COMBAT,
	E_DEATH
};


UCLASS()
class THEMIST_API ASkeletonEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASkeletonEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Status)
		ESkeletonStatus SkeletonStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
		class USphereComponent* FollowSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
		class USphereComponent* AttackSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AI)
		class AAIController* SkeletonController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		class AKnight* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		class UParticleSystem* HitParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		bool bStopAttacking;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float Health;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AI)
		float DamageToPlayer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		class UBoxComponent* ArmCollision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat)
		class UAnimMontage* SkeletonAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		FTimerHandle AttackDelay;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		TSubclassOf<UDamageType> TypeOfDamage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
		FTimerHandle DestroySkeletonDelay;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat)
		bool bCombat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		float MinTimeToNextAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		float MaxTimeToNextAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat)
		float DelaySkeletonDestruction;

	bool bIsTargetAlive;

	void SetSkeletonStatus(ESkeletonStatus Status);

	ESkeletonStatus GetSkeletonStatus();

	UFUNCTION()
		virtual void FollowSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void FollowSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		virtual void AttackSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void AttackSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void Die();

	UFUNCTION(BlueprintCallable)
		void FollowKnightCharacter(class AKnight* Knight);

	UFUNCTION()
		void EnemyAttackOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void EnemyAttackOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
		void DisableWeaponCollision();

	void AttackKnight();

	UFUNCTION(BlueprintCallable)
		void StopCombatMontage();

	UFUNCTION(BlueprintCallable)
		void EndMontageAtDeathPose();

	bool IsSkeletonAlive();

	void ClearActor();

	void PlayAnimationAndJumpToSection(FName Section, float Speed);

};
