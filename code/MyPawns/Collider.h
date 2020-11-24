// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Collider.generated.h"

UCLASS()
class PLEASE_API ACollider : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACollider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UStaticMeshComponent* MeshComponent;


	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USphereComponent* SphereComponent;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = "Mesh")
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
		class UColliderMovementComponent* OurMovementComponent;

	UPROPERTY(EditAnywhere, BluePrintReadWrite , Category = "Movement")
		float MoveSpeed = 30.f;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	FORCEINLINE UStaticMeshComponent* GetMeshComponent() { return MeshComponent; }
	FORCEINLINE USphereComponent* GetSphereComponent() { return SphereComponent; }
	FORCEINLINE UCameraComponent* GetSpringArmComponent() { return Camera; }
	FORCEINLINE USpringArmComponent* GetCameraComponent() { return SpringArm; }

	FORCEINLINE void SetMeshComponent(UStaticMeshComponent* Mesh) { MeshComponent = Mesh; }
	FORCEINLINE void SetSphereComponent(USphereComponent* Sphere) { SphereComponent = Sphere; }
	FORCEINLINE void SetSpringArmComponent(USpringArmComponent* InSpringArm) { SpringArm = InSpringArm; }
	FORCEINLINE void SetCameraComponent(UCameraComponent* InCamera) { Camera = InCamera; }

private:
	void MoveForward(float input);
	void MoveRight(float input);

	void MoveCameraPitch(float axisValue);
	void MoveCameraYaw(float axisValue);

	FVector2D CameraInput;
};
