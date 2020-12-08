// Fill out your copyright notice in the Description page of Project Settings.


#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Enemy.h"
#include "MyPawns/Critter.h"

// Sets default values
ASpawnVolume::ASpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpwaningBox"));
}

// Called when the game starts or when spawned
void ASpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (Actor_1)
		SpawnArray.Add(Actor_1);

	if (Actor_2)
		SpawnArray.Add(Actor_2);

	if (Actor_3)
		SpawnArray.Add(Actor_3);

	if (Actor_4)
		SpawnArray.Add(Actor_4);

	
}

// Called every frame
void ASpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASpawnVolume::GetSpawnPoint()
{
	FVector extent = SpawningBox->GetScaledBoxExtent();
	FVector origin = SpawningBox->GetComponentLocation();

	FVector point = UKismetMathLibrary::RandomPointInBoundingBox(origin, extent);

	return point;
}

TSubclassOf<AActor> ASpawnVolume::GetSpawnActor()
{
	if (SpawnArray.Num() > 0)
	{
		int32 selection = FMath::RandRange(0, SpawnArray.Num() - 1);
		UE_LOG(LogTemp, Warning, TEXT("selection : %d"),selection);

		return SpawnArray[selection];
	}
	return nullptr;
}

void ASpawnVolume::SpawnOurActor_Implementation(UClass* ToSpawn, const FVector & Location)
{
	if (!ToSpawn)return;

	UWorld* world = GetWorld();
	if (!world) return;

	FActorSpawnParameters spawnParams;
	AActor* actor = world->SpawnActor<AActor>(ToSpawn, Location, FRotator(0.f), spawnParams);
	
	AEnemy* enemy = Cast<AEnemy>(actor);

	if (enemy)
	{
		enemy->SpawnDefaultController();

		AAIController* aiCont = Cast<AAIController>(enemy->GetController());

		if (aiCont)
		{
			enemy->AIController = aiCont;
		}
	}

}

