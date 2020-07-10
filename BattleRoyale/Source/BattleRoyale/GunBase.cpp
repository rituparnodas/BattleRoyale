// ©Copyright DevDasTour


#include "GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"

AGunBase::AGunBase()
{
	PrimaryActorTick.bCanEverTick = true;

	SceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));
	RootComponent = SceneComp;

	FirstPersonGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonGun"));
	FirstPersonGun->SetupAttachment(RootComponent);

	ThirdPersonGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdPersonGun"));
	ThirdPersonGun->SetupAttachment(RootComponent);

	SetReplicates(true);
}

void AGunBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

