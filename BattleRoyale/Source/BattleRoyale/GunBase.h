// ©Copyright DevDasTour

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GunBase.generated.h"

UCLASS()
class BATTLEROYALE_API AGunBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGunBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	float Damage = 30.f;

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* SceneComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	FString WeaponName = TEXT("Weapon");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	class USkeletalMeshComponent* FirstPersonGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gun")
	class USkeletalMeshComponent* ThirdPersonGun;
};