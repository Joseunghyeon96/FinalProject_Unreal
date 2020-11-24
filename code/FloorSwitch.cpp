// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorSwitch.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AFloorSwitch::AFloorSwitch()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	//TriggerBox collision setting
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TriggerBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	TriggerBox->SetBoxExtent(FVector(62.f, 62.f, 32.f));

	FloorSwitch = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FloorSwitch"));
	FloorSwitch->SetupAttachment(GetRootComponent());

	Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door"));
	Door->SetupAttachment(GetRootComponent());

	SwitchDelayTime = 2.f;
	bCharacterOnSwitch = false;
}

// Called when the game starts or when spawned
void AFloorSwitch::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AFloorSwitch::OnOverlapEnd);
	
	InitialDoorLocation = Door->GetComponentLocation();
	InitialSwitchLocation = FloorSwitch->GetComponentLocation();
}

// Called every frame
void AFloorSwitch::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorSwitch::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor,
	UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin."));
	bCharacterOnSwitch = true;
	RaiseDoor();
	LowerFloorSwitch();
}

void AFloorSwitch::CloseDoor()
{
	if (bCharacterOnSwitch) return;
	LowerDoor();
	RaiseFloorSwitch();
}

void AFloorSwitch::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End."));
	bCharacterOnSwitch = false;
	GetWorldTimerManager().SetTimer(SwitchHandle, this, &AFloorSwitch::CloseDoor, SwitchDelayTime);
}

void AFloorSwitch::UpdateDoorZLocation(float Z)
{
	FVector newLocation = InitialDoorLocation;
	newLocation.Z += Z;
	Door->SetWorldLocation(newLocation);
}

void AFloorSwitch::UpdateFloorSwitchZLocation(float Z)
{
	FVector newLocation = InitialSwitchLocation;
	newLocation.Z += Z;
	FloorSwitch->SetWorldLocation(newLocation);
}

