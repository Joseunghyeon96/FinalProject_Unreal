// Fill out your copyright notice in the Description page of Project Settings.


#include "MainPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "../Enemy.h"

void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!HUDOverlayAsset) return;

	HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayAsset, TEXT("HUDOverlay"));
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);

	//if (WEnemyHealthBar)
	//{
	//	EnemyHealthBar = CreateWidget<UUserWidget>(this, WEnemyHealthBar);
	//	if (EnemyHealthBar)
	//	{
	//		EnemyHealthBar->AddToViewport();
	//		EnemyHealthBar->SetVisibility(ESlateVisibility::Hidden);
	//	}
	//	FVector2D aligentVec(0.f, 0.f);
	//	EnemyHealthBar->SetAlignmentInViewport(aligentVec);
	//}
	
}

void AMainPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}




