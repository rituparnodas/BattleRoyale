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


	UPROPERTY(ReplicatedUsing=OnRep_Gun, BlueprintReadWrite, Category = "Gameplay")
	class AGunBase* Gun;

	UFUNCTION()
	void OnRep_Gun();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void GiveWeapon(UClass* GunToGive);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<class AGunBase> GunClass;

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