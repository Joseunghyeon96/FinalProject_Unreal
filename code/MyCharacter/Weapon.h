// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EWeaponState: uint8 {
	EWS_Pickup UMETA(DisplayName = "Pickup"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),

	EWS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class PLEASE_API AWeapon : public AItem
{
	GENERATED_BODY()
public:

	AWeapon();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Item")
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Skeletal Mesh")
	class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = Sound)
	class USoundCue* OnEquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Sound)
	class USoundCue* SwingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item | Combat")
	class UBoxComponent* CombatCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	TSubclassOf<UDamageType> DamageTypeClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	AController* WeaponInstigator;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item | Combat")
	float Damage;

protected:

	virtual void BeginPlay() override;
public:
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	UFUNCTION()
	void CombatOnOverlapBegin(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	void CombatOnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void Equip(class AMain* Character);
	void UnEquip(class AMain* Character);

	UFUNCTION(BlueprintCallable)
	void ActivateCollision();

	UFUNCTION(BlueprintCallable)
	void DeactivateCollision();

	FORCEINLINE void SetWeaponState(EWeaponState State) { WeaponState = State; }
	FORCEINLINE EWeaponState GetWeaponState() { return WeaponState; }
	FORCEINLINE void SetInstigator(AController* Inst) { WeaponInstigator = Inst; }
};
