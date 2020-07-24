// ©Copyright DevDasTour

#include "BRGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"

UBRGameInstance::UBRGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MainMenuBPClass(TEXT("/Game/UI/WBP_MainMenu"));
	if (!MainMenuBPClass.Class) return;
	MainMenuClass = MainMenuBPClass.Class;
}

void UBRGameInstance::Init()
{
	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();
	if (SubSystem)
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UBRGameInstance::OnFindSessionComplete);
		}
	}
}

void UBRGameInstance::OnFindSessionComplete(bool Success)
{
	if (Success)
	{
		if (SessionSearch.IsValid())
		{
			if (SessionSearch->SearchResults.Num() > 0)
			{
				FOnlineSessionSearchResult SearchResult = SessionSearch->SearchResults[0];

				if (SearchResult.IsValid())
				{
					
				}
				UE_LOG(LogTemp, Warning, TEXT("Search result Is Greater Than > 0"))
			}
			else
			{
				// TODO Create Session
				UE_LOG(LogTemp, Error, TEXT("Search result Is Less Than < 0"))
			}
			
		}
		UE_LOG(LogTemp, Warning, TEXT("Success"))
	}
}

void UBRGameInstance::TryJoinOrCreateGame()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	if (SessionSearch.IsValid())
	{
		SessionSearch->bIsLanQuery = bUseLan;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		SessionSearch->MaxSearchResults = 100;
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UBRGameInstance::LoadMainMenu()
{
	if (!MainMenuClass) return;
	MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
	if (!MainMenu) return;
	MainMenu->AddToViewport();
	MainMenu->SetMenuInterface(this);
	MainMenu->Setup();
}