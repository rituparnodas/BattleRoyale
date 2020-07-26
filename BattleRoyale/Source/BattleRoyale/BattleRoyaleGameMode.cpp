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

void ABattleRoyaleGameMode::PlayerDied(ABattleRoyaleCharacter* DeadPlayer)
{
	if (HasAuthority())
	{
		ABRPlayerState* PS = Cast<ABRPlayerState>(DeadPlayer->GetPlayerState());
		if (PS)
		{
			PS->Placing = AlivePlayerControllerList.Num();
			ABRPlayerController* PlayerController = Cast<ABRPlayerController>(DeadPlayer->GetController());
			if (PlayerController)
			{
				AlivePlayerControllerList.Remove(PlayerController);
				ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
				if (GS)
				{
					GS->PlayerLeftAlive = AlivePlayerControllerList.Num();
					if (AlivePlayerControllerList.Num() <= 1)
					{
						ABattleRoyaleCharacter* Character = Cast<ABattleRoyaleCharacter>(AlivePlayerControllerList.Last());
						if (Character)
						{
							ABRPlayerState* WinnerPlayerState =  Cast<ABRPlayerState>(Character->GetPlayerState());
							if (WinnerPlayerState)
							{
								WinnerFound(WinnerPlayerState);
							}
						}
					}
				}
			}
		}
	}
}

void ABattleRoyaleGameMode::WinnerFound(ABRPlayerState* Winner)
{
	if (!Winner) return;

	Winner->Placing = 1;

	ABRGameStateBase* GS = Cast<ABRGameStateBase>(GetWorld()->GetGameState());
	if (GS)
	{
		//GS->OnGameOver();
		//GS->OnRep_GameOver();
	}
}