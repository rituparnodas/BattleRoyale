// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABattleRoyaleGameMode();

	UFUNCTION(BlueprintCallable)
	void OnGameStart();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players")
	TArray<class ABRPlayerController*> PlayerControllerList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Players")
		TArray<class ABRPlayerController*> AlivePlayerControllerList;

	void PostLogin(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	void PlayerDied(class ABattleRoyaleCharacter* DeadPlayer);

	UFUNCTION(BlueprintCallable)
		void WinnerFound(class ABRPlayerState* Winner);
};