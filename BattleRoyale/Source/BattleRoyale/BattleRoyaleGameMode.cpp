// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleHUD.h"
#include "BattleRoyaleCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BRPlayerController.h"
#include "BRGameStateBase.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted 
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/PlayerPawn"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();
}

void ABattleRoyaleGameMode::OnGameStart()
{
	for (ABRPlayerController* PlayerController : PlayerControllerList)
	{
		if (PlayerController)
		{
			PlayerController->bIsOnPlane = true;
			ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
			if (GS && GS->AirPlane)
			{
				PlayerController->SetViewTargetWithBlend(GS->AirPlane, 1.f, EViewTargetBlendFunction::VTBlend_Cubic);
				PlayerController->OnGameBegun(); // TO Add Some HUD
			}
		}
	}
}

void ABattleRoyaleGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		if (!GS->bGameStarted)
		{
			ABRPlayerController* PlayerController = Cast<ABRPlayerController>(NewPlayer);
			if (PlayerController)
			{
				PlayerControllerList.Add(PlayerController);
				AlivePlayerControllerList.Add(PlayerController);
				GS->PlayerLeftAlive = AlivePlayerControllerList.Num();
			}
		}
	}
}