// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"
#include "MyCharacter/Main.h"


APickup::APickup()
{
	CoinCount = 1;

}
void APickup::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (!OtherActor) return;

	AMain* main = Cast<AMain>(OtherActor);
	if (!main) return;

	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	main->VariationCoin(CoinCount);
	Destroy();

}

void APickup::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}
