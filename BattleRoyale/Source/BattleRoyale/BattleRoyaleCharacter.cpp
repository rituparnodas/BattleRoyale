// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleCharacter.h"
#include "BattleRoyaleProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GunBase.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/DamageType.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

ABattleRoyaleCharacter::ABattleRoyaleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
}

void ABattleRoyaleCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (GetLocalRole() == ROLE_Authority)
	{
		OnTakeAnyDamage.AddDynamic(this, &ABattleRoyaleCharacter::HandleTakeDamage);
		Health = StartingHealth;
	}

	if (GunClass)
	{
		GiveWeapon(GunClass);
	}
}

void ABattleRoyaleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABattleRoyaleCharacter::OnFire);

	// Loot
	PlayerInputComponent->BindAction("Loot", IE_Pressed, this, &ABattleRoyaleCharacter::Loot);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABattleRoyaleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABattleRoyaleCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABattleRoyaleCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABattleRoyaleCharacter::LookUpAtRate);
}

void ABattleRoyaleCharacter::Loot()
{

}

void ABattleRoyaleCharacter::ServerLoot_Implementation()
{
	Loot();
}

void ABattleRoyaleCharacter::OnRep_Gun()
{
	Gun->ThirdPersonGun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
	Gun->FirstPersonGun->AttachToComponent(GetMesh1P(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "WeaponSocket");
}

void ABattleRoyaleCharacter::GiveWeapon(UClass* GunToGive)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Gun = GetWorld()->SpawnActor<AGunBase>(GunToGive, GetActorTransform(), SpawnParams);
		if (Gun)
		{
			Gun->SetOwner(this);
			Gun->SetInstigator(this);
		}
		OnRep_Gun();
	}
}

void ABattleRoyaleCharacter::OnRep_Reloading()
{
	// TODO Play Animation Monatage

	UE_LOG(LogTemp, Error, TEXT("On Rep Reloading"))
		//OnGunReload(IsLocallyControlled());
		if (Reloading)
		{
			Gun->ReloadAnimation();
		}
}

void ABattleRoyaleCharacter::OnFire()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerFire();
	}
	if (Gun->HasAmmo() && !Reloading)
	{
		SetupFire();
	}
}

void ABattleRoyaleCharacter::SetupFire()
{
	Gun->OnFired();// Decrement A Bullet

	FVector EyeLocation;
	FRotator EyeRotation;
	GetController()->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector ShotDirection = EyeRotation.Vector();
	FVector TraceEnd = EyeLocation + (ShotDirection * TraceRange);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	//QueryParams.AddIgnoredActor(MyOwner);
	QueryParams.bTraceComplex = true;
	QueryParams.bReturnPhysicalMaterial = true;

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, ECollisionChannel::ECC_Visibility, QueryParams))
	{
		DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Magenta, false, 10.f, 0, 1.0f);

		HitActor = Hit.GetActor();
		Victim = Cast<ABattleRoyaleCharacter>(HitActor);
		if (GetLocalRole() == ROLE_Authority)
		{
			if (Victim)
			{
				UGameplayStatics::ApplyDamage(Victim, Gun->Damage, GetInstigatorController(), this, DamageTypes);
				UE_LOG(LogTemp, Warning, TEXT("Victim, Player : %s"), *Victim->GetName())
			}
		}
	}
}

void ABattleRoyaleCharacter::ServerFire_Implementation()
{
	OnFire();
}

void ABattleRoyaleCharacter::OnRep_KilledBy()
{
	// These Are From Who Is Taking Damage
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh1P()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh1P()->SetSimulatePhysics(true);
}

void ABattleRoyaleCharacter::HandleTakeDamage(AActor* DamagedActor, float Damage,
	const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (DamagedActor == DamageCauser) return;

	ModifyHealth(Damage);
	UE_LOG(LogTemp, Warning, TEXT("Health : %f"), Health)
	if (Health <= 0)
	{
		ABattleRoyaleCharacter* Killer = Cast<ABattleRoyaleCharacter>(DamageCauser);
		if (Killer)
		{
			KilledByPlayer(Killer);
		}
		else
		{
			KilledByEnvironment(DamageCauser); // TODO Set As Safe Zone
		}
	}
}

void ABattleRoyaleCharacter::KilledByPlayer(class ABattleRoyaleCharacter* Killer)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		KilledBy = Killer;
		OnRep_KilledBy();
		UE_LOG(LogTemp, Warning, TEXT("By Player : %s"), *KilledBy->GetName())
	}
}

void ABattleRoyaleCharacter::KilledByEnvironment(AActor* EnvActor)
{
	UE_LOG(LogTemp, Warning, TEXT("By Zone"))
}

void ABattleRoyaleCharacter::ModifyHealth(float HealthDelta)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		Health = FMath::Clamp(Health - HealthDelta, 0.f, MaxHealth);
	}
}

void ABattleRoyaleCharacter::OnRep_Health(){}

bool ABattleRoyaleCharacter::ServerFire_Validate(){ return true; }

bool ABattleRoyaleCharacter::ServerLoot_Validate() { return true; }

void ABattleRoyaleCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleRoyaleCharacter, Gun);
	DOREPLIFETIME(ABattleRoyaleCharacter, KilledBy);
	DOREPLIFETIME(ABattleRoyaleCharacter, Health);
	DOREPLIFETIME(ABattleRoyaleCharacter, Reloading);
}

/*=============================================================================================*/
void ABattleRoyaleCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ABattleRoyaleCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

void ABattleRoyaleCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction 
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABattleRoyaleCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ABattleRoyaleCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABattleRoyaleCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ABattleRoyaleCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ABattleRoyaleCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ABattleRoyaleCharacter::EndTouch);
		return true;
	}
	
	return false;
}