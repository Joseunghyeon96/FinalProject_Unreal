// Fill out your copyright notice in the Description page of Project Settings.


#include "Main.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "../Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Weapon.h"
#include "Animation/AnimInstance.h"
#include "MainPlayerController.h"

// Sets default values
AMain::AMain()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create Camera Boom (pulls towards the player if there's a collision
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 600.0f; // Camera follow at this distance
	CameraBoom->bUsePawnControlRotation = true; // Rotate arm based on controller

	// Create Follow Camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Follow Camera"));
	FollowCamera->SetupAttachment(CameraBoom , USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match
	// the controller orientation
	FollowCamera->bUsePawnControlRotation = false;

	//Set turn rates
	BaseTurnRate = 65.f;
	BaseLookUpRate = 65.f;
	
	//Set speed
	RunningSpeed = 650.0f;
	SprintingSpeed = 950.0f;
	
	//Don't rotate when the controller rotates.
	//Let that just affect the camera.
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.f, 0.f);
	GetCharacterMovement()->JumpZVelocity = 460.0f;
	GetCharacterMovement()->AirControl = 0.3f;

	MaxHealth = 100.f;
	CurrentHealth = 65.f;

	MaxStamina = 200.f;
	CurrentStamina = 100.f;

	Coins = 0;

	//Init Enums
	MS = EMovementStatus::EMS_Normal;
	ESS = EStaminaStatus::ESS_Normal;
	
	bCanSprint = true;
	StaminaDrainRate = 25.f;
	MinSprintStamina = 100.f;

	InterpSpeed = 15.f;
	bInterpToEnemy = false;
}

// Called when the game starts or when spawned
void AMain::BeginPlay()
{
	Super::BeginPlay();

	//UKismetSystemLibrary::DrawDebugSphere(this, GetActorLocation() + FVector(0, 0, 75.f), 25.f, 12, FLinearColor::Red, 10.f, 0.4f);
	MainPlayerController = Cast<AMainPlayerController>(GetController());
	
}

// Called every frame
void AMain::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TickStamina(DeltaTime);

	if (bInterpToEnemy && CombatTarget)
	{
		FRotator lookAtYaw = GetLookAtRotationYaw(CombatTarget->GetActorLocation());
		FRotator interpRotation = FMath::RInterpTo(GetActorRotation(), lookAtYaw, DeltaTime, InterpSpeed);
		SetActorRotation(interpRotation);
	}


}

// Called to bind functionality to input
void AMain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);
	//Camera Movement
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	//Character movement
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AMain::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &AMain::StopJumping);
	PlayerInputComponent->BindAction("EquipWeapon", IE_Pressed, this, &AMain::PressKeyF);
	PlayerInputComponent->BindAction("LMB", IE_Pressed, this, &AMain::Attack);
	//PlayerInputComponent->BindAction("UnEquipWeapon", IE_Pressed, this, &AMain::PressKeyG);
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AMain::SetSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AMain::SetRunning);
	PlayerInputComponent->BindAxis("MoveForward", this, &AMain::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMain::MoveRight);
	PlayerInputComponent->BindAxis("TurnRate", this, &AMain::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AMain::LookUpRate);

}

void AMain::MoveForward(float value)
{
	if ((Controller == nullptr) || (value == 0.f) || bAttacking)
		return;

	// Fine out which way is forward
	if (MS != EMovementStatus::EMS_Exhausted)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, value);
	}
}


void AMain::MoveRight(float value)
{
	if ((Controller == nullptr) || (value == 0.f) || MS == EMovementStatus::EMS_Exhausted || bAttacking)
		return;

	// Fine out which way is forward
	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(Direction, value);
}

void AMain::Jump()
{
	if (MS == EMovementStatus::EMS_Exhausted || bAttacking) return;

	ACharacter::Jump();
}

void AMain::StopJumping()
{
	if (MS == EMovementStatus::EMS_Exhausted || bAttacking) return;
	
	ACharacter::StopJumping();
}

void AMain::TurnAtRate(float rate)
{
	AddControllerYawInput(rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AMain::LookUpRate(float rate)
{
	AddControllerPitchInput(rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AMain::VariationHealth(float Amount)
{
	CurrentHealth += Amount;
	if (Amount < 0)
		UGameplayStatics::PlaySound2D(this, HitSound);

	if (CurrentHealth <= 0) Die();
}

void AMain::VariationCoin(int32 Amount)
{
	Coins += Amount;
}

void AMain::Die()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && CombatMontage)
	{
		animInstance->Montage_Play(CombatMontage, 1.0f);
		animInstance->Montage_JumpToSection(FName("Death"),CombatMontage);
	}
}

void AMain::TickStamina(float DeltaTime)
{
	float deltaStamina = StaminaDrainRate * DeltaTime;

	switch (ESS)
	{
	case EStaminaStatus::ESS_Normal:
		bCanSprint = true;
		if (MS == EMovementStatus::EMS_Sprinting)
		{
			if (GetVelocity().Size() >= 0.1f)
			CurrentStamina -= deltaStamina*2;
			else
			{
				CurrentStamina += deltaStamina;
				if (CurrentStamina >= MaxStamina)
					CurrentStamina = MaxStamina;
			}

			if (CurrentStamina <= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_BelowMinimum);
		}
		else
		{
			CurrentStamina += deltaStamina;
			if (CurrentStamina >= MaxStamina)
				CurrentStamina = MaxStamina;
		}
		break;
	case EStaminaStatus::ESS_BelowMinimum:
		if (MS == EMovementStatus::EMS_Sprinting)
		{
			if (GetVelocity().Size() >= 0.1f)
			CurrentStamina -= deltaStamina;
			else
			{
				CurrentStamina += deltaStamina;
				if (CurrentStamina >= MinSprintStamina)
					SetStaminaStatus(EStaminaStatus::ESS_Normal);
			}

			if (CurrentStamina <= 0.f)
			{
				bCanSprint = false;
				SetStaminaStatus(EStaminaStatus::ESS_ExhaustedRecovering);
				SetMovementStatus(EMovementStatus::EMS_Exhausted);
				CurrentStamina = 0.f;
			}
		}
		else
		{
			CurrentStamina += deltaStamina;
			if(CurrentStamina >= MinSprintStamina)
				SetStaminaStatus(EStaminaStatus::ESS_Normal);
		}
		break;
	case EStaminaStatus::ESS_ExhaustedRecovering:
		CurrentStamina += deltaStamina;
		if (CurrentStamina >= MinSprintStamina)
		{
			SetStaminaStatus(EStaminaStatus::ESS_Normal);
			SetMovementStatus(EMovementStatus::EMS_Normal);
		}
		break;
	
	default:
		break;
	}
}

void AMain::SetMovementStatus(EMovementStatus Input)
{
	MS = Input;

	if (MS == EMovementStatus::EMS_Sprinting)
		GetCharacterMovement()->MaxWalkSpeed = SprintingSpeed;
	else if (MS == EMovementStatus::EMS_Normal)
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	else
		GetCharacterMovement()->MaxWalkSpeed = 0.f;

}

void AMain::SetSprinting()
{
	if(bCanSprint && MS != EMovementStatus::EMS_Exhausted)
	SetMovementStatus(EMovementStatus::EMS_Sprinting);
}

void AMain::SetRunning()
{
	if(MS != EMovementStatus::EMS_Exhausted)
	SetMovementStatus(EMovementStatus::EMS_Normal);
}

void AMain::PressKeyF()
{
	if (!ActiveOverlappingItem) return;

	AWeapon* weapon = Cast<AWeapon>(ActiveOverlappingItem);
	if (weapon)
	{
		weapon->Equip(this);
		//ActiveOverlappingItem = nullptr;
	}
		
}

void AMain::PressKeyG()
{
	if (!EquippedWeapon) return;

	AWeapon* weapon = Cast<AWeapon>(ActiveOverlappingItem);
	if (weapon)
	{
		weapon->UnEquip(this);
	}
}

FRotator AMain::GetLookAtRotationYaw(FVector TargetLocation)
{
	FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetLocation);

	FRotator lookAtRotationYaw(0.f, lookAtRotation.Yaw, 0.f);
	
	return lookAtRotationYaw;
}

void AMain::Attack()
{
	if (!EquippedWeapon || bAttacking) return;

	SetInterpToEnemy(true);
	bAttacking = true;
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && CombatMontage)
	{
		animInstance->Montage_Play(CombatMontage,2.0f);
		animInstance->Montage_JumpToSection(FName("Attack_1"), CombatMontage);
	}
	if (EquippedWeapon->SwingSound)
	{
		//UGameplayStatics::PlaySound2D(this, EquippedWeapon->SwingSound);
	}
}

void AMain::AttackEnd()
{
	bAttacking = false;
	SetInterpToEnemy(false);

}

void AMain::SetInterpToEnemy(bool Interp)
{
	bInterpToEnemy = Interp;
}

float AMain::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	VariationHealth(DamageAmount);

	return DamageAmount;
}

