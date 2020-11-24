// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Critter.generated.h"

UCLASS()
class PLEASE_API ACritter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACritter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere,Category = "Pawn Movement")
	float maxSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Variable")
		FVector RelativeLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Variable")
		FRotator RelativeRotation;

private:

	void MoveForward(float value); // move forward(1.f) or move backward(-1.f) 
	void MoveRight(float value); // 

	FVector curVelocity;

	
};
