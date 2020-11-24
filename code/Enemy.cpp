// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Components/SphereComponent.h"
#include "AIController.h"
#include "MyCharacter/Main.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/ProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "Sound/SoundCue.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"
#include "TimerManager.h"
#include "MyCharacter/MainPlayerController.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectableSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectableSphere"));
	DetectableSphere->SetupAttachment(GetRootComponent());
	DetectableSphere->InitSphereRadius(600.0f);

	CombatSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatSphere"));
	CombatSphere->SetupAttachment(GetRootComponent());
	CombatSphere->InitSphereRadius(100.f);

	CombatCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CombatCollision"));
	CombatCollision->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, FName("EnemySocket"));


	bOverlappingCombatSphere = false;
	bAttacking = false;
	bVisibleHealthBar = false;

	MaxHealth = 100.f;
	Health = 100.f;
	AttackMinTime = 0.5f;
	AttackMaxTime = 3.5f;
	
	DeathDelay = 3.0f;
	Damage = 10.f;

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(GetMesh());
	HPBarWidget->SetHiddenInGame(false);

}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	AIController = Cast<AAIController>(GetController());

	DetectableSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::DectectableSphereOnOverlapBegin);
	DetectableSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::DectectableSphereOnOverlapEnd);

	CombatSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapBegin);
	CombatSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatSphereOnOverlapEnd);


	CombatCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapBegin);
	CombatCollision->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatOnOverlapEnd);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CombatCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CombatCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 180.f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	if (WEnemyHealthBar)
	{
		HPBarWidget->SetWidgetClass(WEnemyHealthBar);
		HPBarWidget->SetDrawSize(FVector2D(250.0f, 15.0f));
	}

	HPBar = Cast<UProgressBar>(HPBarWidget->GetUserWidgetObject()->GetWidgetFromName(TEXT("EnemyHealthBar")));
	HPBar->SetPercent(Health / MaxHealth);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::DectectableSphereOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IsAlive()) return;

	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);
		bVisibleHealthBar = true;

		if (main) {
			bVisibleHealthBar = true;
			HPBarWidget->SetHiddenInGame(false);
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
			MoveToTarget(main);
		}
	}
}

void AEnemy::DectectableSphereOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive()) return;

	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);

		if (main) {
			bVisibleHealthBar = false;
			HPBarWidget->SetHiddenInGame(true);
			SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Idle);
			if (AIController)
			{
				AIController->StopMovement();
			}
		}
	}
}

void AEnemy::CombatSphereOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (!IsAlive()) return;

	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);


		if (main) {
			main->SetCombatTarget(this);
			CombatTarget = main;
			bOverlappingCombatSphere = true;
			Attack();
		}
	}
}

void AEnemy::CombatSphereOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive()) return;

	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);

		if (main) {
			bOverlappingCombatSphere = false;
			if(main->CombatTarget == this)
				main->SetCombatTarget(nullptr);

			if (EnemyMovementStatus != EEnemyMovementStatus::EMS_Attacking)
			{
				CombatTarget = nullptr;
				MoveToTarget(main);
			}
			GetWorldTimerManager().ClearTimer(AttackTimer);
		}
	}
}

void AEnemy::CombatOnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!IsAlive()) return;

	if (OtherActor)
	{
		AMain* main = Cast<AMain>(OtherActor);

		if (main) {
			if (main->HitParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), main->HitParticles, OtherComp->GetComponentLocation(), FRotator(0.f), false);
			}
			if (DamageTypeClass)
			{
				UGameplayStatics::ApplyDamage(main, -Damage, AIController, this, DamageTypeClass);
			}
		}
	}
}

void AEnemy::CombatOnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	if (!IsAlive()) return;
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	Health += DamageAmount;
	HPBar->SetPercent(Health / MaxHealth);
	if (Health <= 0.f)
		Die();

	return DamageAmount;
}

void AEnemy::MoveToTarget(AMain * Target)
{

	if (!AIController || !IsAlive()) return;

	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);

	FAIMoveRequest moveRequest;
	moveRequest.SetGoalActor(Target);
	moveRequest.SetAcceptanceRadius(10.0f);

	FNavPathSharedPtr navPath;
	
	AIController->MoveTo(moveRequest, &navPath);

	//TArray<FNavPathPoint> pathPoints = navPath->GetPathPoints();
	//for (auto point : pathPoints)
	//{
	//	UKismetSystemLibrary::DrawDebugSphere(this, point.Location, 25.f, 8, FLinearColor::Red, 10.f, 0.4f);
	//}
}

void AEnemy::Die()
{
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (animInstance && CombatMontage)
	{
		animInstance->Montage_Play(CombatMontage, 1.0f);
		animInstance->Montage_JumpToSection(FName("Death"),CombatMontage);
	}
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Dead);

	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DetectableSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CombatSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AEnemy::Attack()
{
	if (!IsAlive()) return;

	if (AIController)
	{
		AIController->StopMovement();
		SetEnemyMovementStatus(EEnemyMovementStatus::EMS_Attacking);
	}

	if (!bAttacking)
	{
		bAttacking = true;
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance)
		{
			animInstance->Montage_Play(CombatMontage, 1.3f);
			animInstance->Montage_JumpToSection(FName("Attack1"), CombatMontage);
			
		}
		if (AttackSound)
		{
			UGameplayStatics::PlaySound2D(this, AttackSound);
		}
	}
}

void AEnemy::AttackEnd()
{
	bAttacking = false;
	SetEnemyMovementStatus(EEnemyMovementStatus::EMS_MoveToTarget);
	if (bOverlappingCombatSphere)
	{
		float attackTime = FMath::FRandRange(AttackMinTime, AttackMaxTime);
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, attackTime);
	}
}

void AEnemy::DeathEnd()
{
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;

	GetWorldTimerManager().SetTimer(DeathTimer, this, &AEnemy::Disappear, DeathDelay);
}

void AEnemy::Disappear()
{
	Destroy();
}

bool AEnemy::IsAlive()
{
	return GetEnemyMovementStatus()!=EEnemyMovementStatus::EMS_Dead;
}

void AEnemy::ActivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeactivateCollision()
{
	CombatCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}