// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BattleRoyaleCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class ABattleRoyaleCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = "Mesh")
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;


public:
	ABattleRoyaleCharacter();

protected:
	virtual void BeginPlay();

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Mesh")
	class USkeletalMeshComponent* Mesh1P;

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ABattleRoyaleProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;
	
	//=======================Gun=================================

	UPROPERTY(ReplicatedUsing=OnRep_Gun, BlueprintReadWrite, Category = "Gameplay")
	class AGunBase* Gun;

	UFUNCTION()
	void OnRep_Gun();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void GiveWeapon(UClass* GunToGive, int32 CurrentAmmo, int32 CurrentBagAmmo);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AGunBase> GunClass;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Reloading, BlueprintReadWrite, Category = "Weapon")
		bool Reloading = false;

	UFUNCTION()
	void OnRep_Reloading();

	UFUNCTION(BlueprintImplementableEvent)
	void OnGunReload(bool IsLocalPlayer);

	UPROPERTY(Replicated, BlueprintReadWrite, Category = "Pickup")
	class APickupGun* CurrentGunPickup;

	UFUNCTION(BlueprintCallable)
		void DropCurrentWeapon();

	UFUNCTION(BlueprintPure)
	FTransform GetPickupSpawnTransform();

	// *********************************Health*************************************
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Health")
		float Health = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float StartingHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float MaxHealth = 150.f;

	UFUNCTION()
		void OnRep_Health();

	UFUNCTION(BlueprintCallable)
		void ModifyHealth(float HealthDelta);

protected:
	
	/** Fires a projectile. */
	void OnFire();

	void SetupFire();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LineTrace")
	float TraceRange = 10000;

	UPROPERTY(ReplicatedUsing=OnRep_KilledBy, BlueprintReadWrite, Category = "Gameplay")
		class ABattleRoyaleCharacter* KilledBy;

	UFUNCTION()
		void OnRep_KilledBy();

	AActor* HitActor;

	UPROPERTY(BlueprintReadOnly)
	class ABattleRoyaleCharacter* Victim;

	UFUNCTION(BlueprintCallable)
	void KilledByPlayer(class ABattleRoyaleCharacter* Killer);

	UFUNCTION(BlueprintCallable)
	void KilledByEnvironment(AActor* EnvActor);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UDamageType> DamageTypes;

	UFUNCTION()
		void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
		void HandleLocalDeath();
	

	//******************************************Loot*****************************************
	void Loot();

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerLoot();

public:

	//*****************************************Flying****************************************
	UFUNCTION(BlueprintCallable)
	void SetFlying(bool bIsFlying);

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Movement")
	bool bFlying;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMovement();

protected:

	//========================================================================================
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:

	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }

	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};