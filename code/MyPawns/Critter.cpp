// Fill out your copyright notice in the Description page of Project Settings.


#include "Critter.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values
ACritter::ACritter()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	MeshComponent->SetupAttachment(GetRootComponent());
	Camera->SetupAttachment(GetRootComponent());
	Camera->SetRelativeLocation(RelativeLocation);
	Camera->SetRelativeRotation(RelativeRotation);


	curVelocity = FVector(0.f);
	maxSpeed = 100.f;
}

// Called when the game starts or when spawned
void ACritter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACritter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector newLocation = GetActorLocation() + (curVelocity * DeltaTime);
	SetActorLocation(newLocation);

}

// Called to bind functionality to input
void ACritter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &ACritter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &ACritter::MoveRight);
}

void ACritter::MoveForward(float value)
{
	curVelocity.X = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}

void ACritter::MoveRight(float value)
{
	curVelocity.Y = FMath::Clamp(value, -1.f, 1.f) * maxSpeed;
}

