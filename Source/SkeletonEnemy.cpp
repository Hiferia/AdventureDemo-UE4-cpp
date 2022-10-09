// Fill out your copyright notice in the Description page of Project Settings.


#include "SkeletonEnemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "Knight.h"
#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"

// Sets default values
ASkeletonEnemy::ASkeletonEnemy()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FollowSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Follow Sphere"));
	FollowSphere->SetupAttachment(GetRootComponent());

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(GetRootComponent());

	ArmCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Arm"));
	ArmCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));

	FollowSphere->InitSphereRadius(488.0f);
	AttackSphere->InitSphereRadius(83.0f);

	SkeletonStatus = ESkeletonStatus::E_IDLE;

	bStopAttacking = false;
	bCombat = false;
	bIsTargetAlive = false;

	Health = 100.0f;
	MaxHealth = 100.0f;

	DamageToPlayer = 10.0f;

	MinTimeToNextAttack = 0.4f;
	MaxTimeToNextAttack = 1.8f;

	DelaySkeletonDestruction = 50.0f;
}

// Called when the game starts or when spawned
void ASkeletonEnemy::BeginPlay()
{
	Super::BeginPlay();

	SkeletonController = Cast<AAIController>(GetController());

	FollowSphere->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonEnemy::FollowSphereOnOverlapBegin);

	FollowSphere->OnComponentEndOverlap.AddDynamic(this, &ASkeletonEnemy::FollowSphereOnOverlapEnd);

	AttackSphere->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonEnemy::AttackSphereOnOverlapBegin);

	AttackSphere->OnComponentEndOverlap.AddDynamic(this, &ASkeletonEnemy::AttackSphereOnOverlapEnd);

	ArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ArmCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ArmCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ArmCollision->SetCollisionResponseToChannel
	(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	ArmCollision->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonEnemy::EnemyAttackOnOverlapBegin);

	ArmCollision->OnComponentEndOverlap.AddDynamic(this, &ASkeletonEnemy::EnemyAttackOnOverlapEnd);

	GetMesh()->SetCollisionResponseToChannel
	(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

}

// Called every frame
void ASkeletonEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASkeletonEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ASkeletonEnemy::SetSkeletonStatus(ESkeletonStatus Status)
{
	SkeletonStatus = Status;
}

ESkeletonStatus ASkeletonEnemy::GetSkeletonStatus()
{
	return SkeletonStatus;
}

void ASkeletonEnemy::FollowSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsSkeletonAlive())
	{
		AKnight* Knight = Cast<AKnight>(OtherActor);

		if (Knight)
		{
			FollowKnightCharacter(Knight);
		}
	}

}

void ASkeletonEnemy::FollowSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AKnight* Knight = Cast<AKnight>(OtherActor);

		if (Knight)
		{
			bIsTargetAlive = false;
			SetSkeletonStatus(ESkeletonStatus::E_IDLE);

			if (SkeletonController)
			{
				SkeletonController->StopMovement();
			}
		}
	}
}

void ASkeletonEnemy::AttackSphereOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && IsSkeletonAlive())
	{
		AKnight* Knight = Cast<AKnight>(OtherActor);

		if (Knight)
		{
			bIsTargetAlive = true;
			Target = Knight;
			bStopAttacking = true;
			AttackKnight();
		}
	}
}

void ASkeletonEnemy::AttackSphereOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && IsSkeletonAlive())
	{
		AKnight* Knight = Cast<AKnight>(OtherActor);

		if (Knight)
		{
			bStopAttacking = false;
			FollowKnightCharacter(Knight);
			Target = nullptr;
			GetWorldTimerManager().ClearTimer(AttackDelay);
		}
	}
}

float ASkeletonEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0.0f)
	{
		Die();
	}
	return DamageAmount;
}

void ASkeletonEnemy::Die()
{
	PlayAnimationAndJumpToSection("Death", 1.0f);

	SetSkeletonStatus(ESkeletonStatus::E_DEATH);

	ArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FollowSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASkeletonEnemy::FollowKnightCharacter(AKnight* Knight)
{
	SetSkeletonStatus(ESkeletonStatus::E_RUN);

	if (SkeletonController)
	{
		FAIMoveRequest MoveRequest;
		MoveRequest.SetGoalActor(Knight);
		MoveRequest.SetAcceptanceRadius(7.0f);
		FNavPathSharedPtr NavPathSharedPtr;
		SkeletonController->MoveTo(MoveRequest, &NavPathSharedPtr);
	}
}

void ASkeletonEnemy::EnemyAttackOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AKnight* Knight = Cast<AKnight>(OtherActor);
		if (Knight)
		{
			if (Knight->HitParticle)
			{
				const USkeletalMeshSocket* HitSocket = GetMesh()->GetSocketByName("HitSocket");
				if (HitSocket)
				{
					FVector SocketLocation = HitSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Knight->HitParticle, SocketLocation, FRotator(0.0f), false);
				}
			}
			if (TypeOfDamage)
			{
				// this will call the TakeDamage() function inside the Knight
				UGameplayStatics::ApplyDamage(Knight, DamageToPlayer, SkeletonController, this, TypeOfDamage);
			}
		}
	}
}

void ASkeletonEnemy::EnemyAttackOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//Do something
}

void ASkeletonEnemy::EnableWeaponCollision()
{
	ArmCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ASkeletonEnemy::DisableWeaponCollision()
{
	ArmCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ASkeletonEnemy::AttackKnight()
{
	if (IsSkeletonAlive() && bIsTargetAlive)
	{
		if (SkeletonController)
		{
			SkeletonController->StopMovement();
			SetSkeletonStatus(ESkeletonStatus::E_COMBAT);
		}

		if (!bCombat)
		{
			bCombat = true;
			PlayAnimationAndJumpToSection("Attack1", 1.3f);
		}
	}
}

void ASkeletonEnemy::StopCombatMontage()
{
	bCombat = false;

	if (bStopAttacking)
	{
		float NextAttackTime = FMath::FRandRange(MinTimeToNextAttack, MaxTimeToNextAttack);
		GetWorldTimerManager().SetTimer(AttackDelay, this, &ASkeletonEnemy::AttackKnight, NextAttackTime);
	}
}

void ASkeletonEnemy::EndMontageAtDeathPose()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetWorldTimerManager().SetTimer(DestroySkeletonDelay, this, &ASkeletonEnemy::ClearActor, DelaySkeletonDestruction);
}

bool ASkeletonEnemy::IsSkeletonAlive()
{
	return GetSkeletonStatus() != ESkeletonStatus::E_DEATH;
}

void ASkeletonEnemy::ClearActor()
{
	Destroy();
}

void ASkeletonEnemy::PlayAnimationAndJumpToSection(FName Section, float Speed)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(SkeletonAnimMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(Section, SkeletonAnimMontage);
	}
}
