// ©Copyright DevDasTour

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "PickupGun.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API APickupGun : public APickup
{
	GENERATED_BODY()	

public:
	
	virtual void PickupEntered(class ABattleRoyaleCharacter* Character);

	virtual void PickupLeft(class ABattleRoyaleCharacter* Character);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun", meta = (InstanceEditable = "true"))
	TSubclassOf<class AGunBase> WeaponToGive;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gun", meta = (ExposeOnSpawn = "true"), meta = (InstanceEditable = "true"))
		int32 CurrentAmmoInClip;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Gun", meta = (ExposeOnSpawn = "true"), meta = (InstanceEditable = "true"))
		int32 CurrentSpareAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun", meta = (ExposeOnSpawn = "true"), meta = (InstanceEditable = "true"))
		bool bDropByPlayer;

};
