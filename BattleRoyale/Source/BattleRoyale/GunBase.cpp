// ©Copyright DevDasTour


#include "GunBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Net/UnrealNetwork.h"
#include "BattleRoyaleCharacter.h"
#include "Kismet/KismetSystemLibrary.h"

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
	
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentAmmo = ClipSize;
		CurrentBagAmmo = StartingBagAmmo;
		
		ABattleRoyaleCharacter* GetTheOwner = Cast<ABattleRoyaleCharacter>(GetOwner());
		if (GetTheOwner)
		{
			CurrentOwner = GetTheOwner;
		}
	}
}

void AGunBase::GiveAmmo(int32 AmmoToGive)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		int32 FreeMagazine = ClipSize - CurrentAmmo;
		int32 SelectAmmoToGive = FMath::Min(FreeMagazine, AmmoToGive);
		CurrentAmmo += SelectAmmoToGive;

		int32 AmmoToStore = AmmoToGive - SelectAmmoToGive;
		if (AmmoToStore > 0)
		{
			int32 CalculateBagSpace = MaxBagAmmo - CurrentBagAmmo;
			int32 SelectAmmoToStore = FMath::Min(CalculateBagSpace, AmmoToStore);
			CurrentBagAmmo += SelectAmmoToStore;
		}
	}
}

void AGunBase::OnFired()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		CurrentAmmo--;
		UE_LOG(LogTemp, Warning, TEXT("----------------"))
		if (CurrentAmmo <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("*****Reloading"))
			Reload();
		}
	}
}

void AGunBase::Reload()
{
	//CurrentOwner->Reloading = true;
	//TODO Set Delay To Play The Animation
	int32 SelectAmmoToReload = FMath::Min(CurrentBagAmmo, ClipSize);
	CurrentAmmo = SelectAmmoToReload;
	int32 AmmoLeftAfterReload = CurrentBagAmmo - SelectAmmoToReload;
	CurrentBagAmmo = FMath::Max(AmmoLeftAfterReload, 0);
	//CurrentOwner->Reloading = false;
}

void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGunBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGunBase, CurrentAmmo);
	DOREPLIFETIME(AGunBase, CurrentBagAmmo);
	DOREPLIFETIME(AGunBase, CurrentOwner);
}