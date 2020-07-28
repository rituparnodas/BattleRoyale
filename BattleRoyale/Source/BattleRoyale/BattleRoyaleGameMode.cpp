// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyaleHUD.h"
#include "BattleRoyaleCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "BRPlayerController.h"
#include "BRGameStateBase.h"
#include "BRPlayerState.h"

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
			ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
			if (GS && GS->AirPlane)
			{
				PlayerController->bIsOnPlane = true;
				PlayerController->OnRep_IsOnPlane();
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
		ABRPlayerController* PlayerController = Cast<ABRPlayerController>(NewPlayer);
		if (PlayerController)
		{
			if (!GS->bGameStarted)
			{
				PlayerControllerList.Add(PlayerController);
				AlivePlayerControllerList.Add(PlayerController);
				GS->PlayerLeftAlive = AlivePlayerControllerList.Num();
			}
			else
			{
				ABRPlayerState* PS = Cast<ABRPlayerState>(PlayerController->PlayerState);
				PS->Placing = AlivePlayerControllerList.Num();
			}
		}
	}
}

void ABattleRoyaleGameMode::PlayerDied(ABRPlayerController* DeadPlayerController)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		if (!DeadPlayerController) return;
		ABRPlayerState* PS = Cast<ABRPlayerState>(DeadPlayerController->PlayerState);
		if (!PS) return;
		PS->Placing = AlivePlayerControllerList.Num();

		AlivePlayerControllerList.Remove(DeadPlayerController);

		ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
		if (!GS) return;
		GS->PlayerLeftAlive = AlivePlayerControllerList.Num();
		if (AlivePlayerControllerList.Num() <= 1)
		{
			ABRPlayerState* WinnerPlayerState = Cast<ABRPlayerState>(AlivePlayerControllerList.Last()->PlayerState);
			if (WinnerPlayerState)
			{
				WinnerFound(WinnerPlayerState);
			}
		}

		UpdatePlacingForAllPlayers();
	}
}

void ABattleRoyaleGameMode::WinnerFound(ABRPlayerState* Winner)
{
	if (!Winner) return;
	Winner->Placing = 1;

	ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->OnGameOver();
		GS->OnRep_GameOver();
	}
}


void ABattleRoyaleGameMode::UpdatePlacingForAllPlayers()
{
	// Set Placing For All Player
	for (ABRPlayerController* PC : AlivePlayerControllerList)
	{
		if (!PC) return;

		ABRPlayerState* PlayerState = Cast<ABRPlayerState>(PC->PlayerState);
		if (!PlayerState) return;
		PlayerState->Placing = AlivePlayerControllerList.Num();
	}
}