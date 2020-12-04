// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Main.generated.h"

UENUM(BlueprintType)
enum class EMovementStatus : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),
	EMS_Exhausted UMETA(DisplayName = "Exhausted"),
	EMS_Dead UMETA(DisplayName= "Dead"),

	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaStatus : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_BelowMinimum UMETA(DisplayName = "BelowMinimum"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),


	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PLEASE_API AMain : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMain();


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		bool bCanComboAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		bool bComboCheck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		int32 CurrentCombo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat)
		int32 MaxCombo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items)
	class AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items)
	class AItem* ActiveOverlappingItem;

	// Movement Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite , Category = "Enums")
	EMovementStatus MS;

	// Stamina Status
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Enums")
	EStaminaStatus ESS;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float StaminaDrainRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	float MinSprintStamina;

	// Camera boom positioning the camera behind the player
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	// Follow Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	//to scale turning functions for the camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speed")
	float RunningSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Speed")
	float SprintingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
		bool bCanSprint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Anims")
	bool bAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anims")
	class UAnimMontage* CombatMontage;
	/*
	Player Stats
	*/
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Player Stats")
		float MaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Stats")
		float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Player Stats")
		float MaxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
		float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	class UParticleSystem* HitParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles")
	class UParticleSystem* HealParticles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* HitSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	class USoundCue* HealSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player Stats")
	int32 Coins;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	class AEnemy* CombatTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat")
	FVector CombatTargetLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller")
	class AMainPlayerController* MainPlayerController;

	float InterpSpeed;

	bool bInterpToEnemy;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Called for forward or backward input
	void MoveForward(float value);

	// Called for side input
	void MoveRight(float value);

	virtual void Jump() override;
	virtual void StopJumping() override;

	// Called via input to turn at a given rate
	// @param rate This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	void TurnAtRate(float rate);

	// Called via input to look up/down at a given rate
	// @param rate This is a normalized rate, i.e. 1.0 means 100% of desired look up/down rate
	void LookUpRate(float rate);

	UFUNCTION()
		void VariationCoin(int32 Amount);

	UFUNCTION()
		void Die();

	void TickStamina(float DeltaTime);
	void SetMovementStatus(EMovementStatus Input);

	void SetSprinting();
	void SetRunning();

	void PressKeyF();
	void PressKeyG();

	FRotator GetLookAtRotationYaw(FVector TargetLocation);

	void Attack();
	void ComboAttack();

	UFUNCTION(BlueprintCallable)
	void ComboCheck();

	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void DeathEnd();

	void SetInterpToEnemy(bool Interp);

	void SimpleTakeDamage(float DamageAmount);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	FORCEINLINE void SetStaminaStatus(EStaminaStatus Input) { ESS = Input; }
	FORCEINLINE void SetCombatTarget(AEnemy* Target) { CombatTarget = Target; }
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE void SetEquippedWeapon(AWeapon* WeaponToSet) { EquippedWeapon = WeaponToSet; }
	FORCEINLINE AWeapon* GetEquippedWeapon() { return EquippedWeapon; }
	FORCEINLINE void SetActiveOverlappingItem(AItem* Item) { ActiveOverlappingItem = Item; }

};
