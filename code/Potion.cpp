// Fill out your copyright notice in the Description page of Project Settings.


#include "Potion.h"
#include "MyCharacter/Main.h"
#include "Kismet/GameplayStatics.h"

APotion::APotion()
{
	HealAmount = -15.0f;
}

void APotion::OnOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

	if (!OtherActor) return;

	AMain* main = Cast<AMain>(OtherActor);
	if (!main) return;

	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	UGameplayStatics::ApplyDamage(OtherActor, HealAmount, nullptr, this, DamageTypeClass);
	Destroy();

}

void APotion::OnOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

}