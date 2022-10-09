// Fill out your copyright notice in the Description page of Project Settings.


#include "Knight.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Sound/SoundCue.h"
#include "SkeletonEnemy.h"
#include "TimerManager.h"


// Sets default values
AKnight::AKnight()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// SpringArm and Camera
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(GetRootComponent());
	CameraArm->bUsePawnControlRotation = true;
	CameraArm->TargetArmLength = 150.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = true;


	// SwordCollisionBox
	SwordCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Sword"));
	SwordCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("WeaponSocket"));

	CharacterTurnRate = 60.0f;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);

	MaxHealth = 100.0f;
	MaxStamina = 100.0f;
	MaxMana = 60.0f;

	Health = 100.0f;
	Stamina = 100.0f;
	Mana = 0.0f;

	WalkSpeed = 200.0f;
	RunSpeed = 500.0f;

	KnightLocomotionStatus = EKnightLocomotionStatus::E_WALK;
	bRunKeyPressed = false;
	bMoveForwardBackward = false;
	bMoveRightLeft = false;

	KnightStaminaStatus = EKnightStaminaStatus::ES_NORMAL;
	StaminaUsage = 15.0f;
	MinimumRunStamina = 30.0f;

	bLMB = false;
	bCombat = false;
	bRoll = false;

	SwordDamage = 40.0f;
}

// Called when the game starts or when spawned
void AKnight::BeginPlay()
{
	Super::BeginPlay();

	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SwordCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	SwordCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SwordCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SwordCollision->OnComponentBeginOverlap.AddDynamic(this, &AKnight::SwordOnOverlapBegin);
	SwordCollision->OnComponentEndOverlap.AddDynamic(this, &AKnight::SwordOnOverlapEnd);

	SetInstigator(AKnight::GetController());
}

// Called every frame
void AKnight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("KeyPressed: %s"), KnightLocomotionStatus == EKnightLocomotionStatus::E_RUN? "Runn" : "falsa");
	if (KnightLocomotionStatus == EKnightLocomotionStatus::E_DEATH)
	{
		return;
	}

	float DeltaStamina = StaminaUsage * DeltaTime;

	switch (KnightStaminaStatus)
	{
	case EKnightStaminaStatus::ES_NORMAL:
		if (bRunKeyPressed)
		{
			Stamina -= DeltaStamina;
			if (Stamina <= MinimumRunStamina)
			{
				SetKnightStaminaStatus(EKnightStaminaStatus::ES_UNDERMINIMUM);
			}
			if (bMoveForwardBackward || bMoveRightLeft)
			{
				SetKnightLocomotionStatus(EKnightLocomotionStatus::E_RUN);
			}
			else
			{
				SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
			}
			
		}
		else
		{
			Stamina += DeltaStamina;
			if (Stamina > MaxStamina)
			{
				Stamina = MaxStamina;
			}
			SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
		}
		break;
	case EKnightStaminaStatus::ES_UNDERMINIMUM:
		if (bRunKeyPressed)
		{
			Stamina -= DeltaStamina;
			if (Stamina <= 0.0f)
			{
				Stamina = 0.0f;
				SetKnightStaminaStatus(EKnightStaminaStatus::ES_ZERO);
			}
			else
			{
				if (bMoveForwardBackward || bMoveRightLeft)
				{
					SetKnightLocomotionStatus(EKnightLocomotionStatus::E_RUN);
				}
				else
				{
					SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
				}
			}
		}
		else
		{
			Stamina += DeltaStamina;
			if (Stamina >= MinimumRunStamina)
			{
				SetKnightStaminaStatus(EKnightStaminaStatus::ES_NORMAL);
			}
			SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
		}
		break;
	case EKnightStaminaStatus::ES_ZERO:
		if (bRunKeyPressed)
		{
			Stamina = 0.0f;
		}
		else
		{
			SetKnightStaminaStatus(EKnightStaminaStatus::ES_RECOVER);
			Stamina += DeltaStamina;
		}
		SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
		break;
	case EKnightStaminaStatus::ES_RECOVER:
		Stamina += DeltaStamina;
		if (Stamina >= MinimumRunStamina)
		{
			SetKnightStaminaStatus(EKnightStaminaStatus::ES_NORMAL);
		}
		SetKnightLocomotionStatus(EKnightLocomotionStatus::E_WALK);
		break;
	}

}

// Called to bind functionality to input
void AKnight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForwardBackward", this, &AKnight::MoveCharacterForwardBackward);
	PlayerInputComponent->BindAxis("MoveRightLeft", this, &AKnight::MoveCharacterRightOrLeft);
	PlayerInputComponent->BindAxis("TurnRightLeft", this, &AKnight::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUpDown", this, &AKnight::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("TurnRightLeftAtRate", this, &AKnight::TurnCharacterRightOrLeftAtRate);
	PlayerInputComponent->BindAxis("LookUpDownAtRate", this, &AKnight::LookCharacterUpOrDownAtRate);
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AKnight::RunKeyPressed);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AKnight::RunKeyNotPressed);
	PlayerInputComponent->BindAction("Roll", IE_Pressed, this, &AKnight::RollKeyPressed);
	PlayerInputComponent->BindAction("Roll", IE_Released, this, &AKnight::RollKeyNotPressed);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Pressed, this, &AKnight::LeftMouseButtonActive);
	PlayerInputComponent->BindAction("LeftMouseButton", IE_Released, this, &AKnight::LeftMouseButtonNotActive);
}

void AKnight::MoveCharacterForwardBackward(float Value)
{
	bMoveForwardBackward = false;

	if (Controller != nullptr && Value != 0.1f && KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		// Get the direction of the rotation
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value * 0.2f);

		bMoveForwardBackward = true;
	}
}

void AKnight::MoveCharacterRightOrLeft(float Value)
{
	bMoveRightLeft = false;

	if (Controller != nullptr && Value != 0.0f && KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

		// Get the direction of the rotation
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value * 0.2f);

		bMoveRightLeft = true;
	}
}

void AKnight::LookCharacterUpOrDownAtRate(float Rate)
{
	AddControllerPitchInput(GetWorld()->GetDeltaSeconds() * CharacterTurnRate * Rate);
}

void AKnight::TurnCharacterRightOrLeftAtRate(float Rate)
{
	AddControllerYawInput(GetWorld()->GetDeltaSeconds() * CharacterTurnRate * Rate);
}

USpringArmComponent* AKnight::GetCameraArm() const
{
	return CameraArm;
}

UCameraComponent* AKnight::GetCamera() const
{
	return Camera;
}

void AKnight::DecreaseHealth(float Amount)
{
	Health -= Amount;

	if (Health <= 0.0f)
	{
		Die();
	}
}

void AKnight::IncreaseHealth(float Amount)
{
	Health += Amount;

	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
}

void AKnight::DecreaseMana(float Amount)
{
	Mana -= Amount;

	if (Mana <= 0.0f)
	{
		Mana = 0.0f;
	}
}

void AKnight::IncreaseMana(float Amount)
{
	Mana += Amount;

	if (Mana > MaxMana)
	{
		Mana = MaxMana;
	}
}

void AKnight::Die()
{
	PlayAnimationAndJumpToSection("Death", 1.0f);
	SetKnightLocomotionStatus(EKnightLocomotionStatus::E_DEATH);
}

void AKnight::SetKnightLocomotionStatus(EKnightLocomotionStatus Status)
{
	KnightLocomotionStatus = Status;

	if (KnightLocomotionStatus == EKnightLocomotionStatus::E_RUN)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	}
}

void AKnight::SetKnightStaminaStatus(EKnightStaminaStatus Status)
{
	KnightStaminaStatus = Status;
}

void AKnight::RunKeyPressed()
{
	bRunKeyPressed = true;
}

void AKnight::RunKeyNotPressed()
{
	bRunKeyPressed = false;
}

void AKnight::RollKeyPressed()
{
	bRollKeyPressed = true;

	if (KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		StartRollMontage();
	}
}

void AKnight::RollKeyNotPressed()
{
	bRollKeyPressed = false;
}

void AKnight::LeftMouseButtonActive()
{
	bLMB = true;
	if (KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		StartCombatMontage();
	}
}

void AKnight::LeftMouseButtonNotActive()
{
	bLMB = false;
}

void AKnight::StartCombatMontage()
{
	if (!bCombat && KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		bCombat = true;
		PlayAnimationAndJumpToSection("Attack1", 1.4f);
	}
}

void AKnight::StartRollMontage()
{
	if (!bRoll && KnightLocomotionStatus != EKnightLocomotionStatus::E_DEATH)
	{
		bRoll = true;
		PlayAnimationAndJumpToSection("Final", 1.4f);
	}
}

void AKnight::StopRolling()
{
	bRollKeyPressed = false;
}

float AKnight::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	DecreaseHealth(DamageAmount);
	return 0.0f;
}

void AKnight::StopCombatMontage()
{
	bCombat = false;
	StopAnimation();
}

void AKnight::StopRollMontage()
{
	bRoll = false;
	StopAnimation();
}

void AKnight::SwordOnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		ASkeletonEnemy* Skeleton = Cast<ASkeletonEnemy>(OtherActor);
		if (Skeleton)
		{
			if (Skeleton->HitParticle)
			{
				const USkeletalMeshSocket* HitSocket = GetMesh()->GetSocketByName("ParticleSocket");
				if (HitSocket)
				{
					FVector SocketLocation = HitSocket->GetSocketLocation(GetMesh());
					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Skeleton->HitParticle, SocketLocation, FRotator(0.0f), false);
				}
			}
			if (TypeOfDamage)
			{
				UGameplayStatics::ApplyDamage(Skeleton, SwordDamage, Controller, this, TypeOfDamage);
			}
		}
	}
}

void AKnight::SwordOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AKnight::EnableWeaponCollision()
{
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AKnight::DisableWeaponCollision()
{
	SwordCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AKnight::SetInstigator(AController* Insti)
{
	SwordInstigator = Insti;
}

void AKnight::EndAtDeath()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
}

void AKnight::PlayAnimationAndJumpToSection(FName Section, float Speed)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(KnightAnimMontage, 1.0f);
		AnimInstance->Montage_JumpToSection(Section, KnightAnimMontage);
	}
}

void AKnight::StopAnimation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.0f);
	}
}
