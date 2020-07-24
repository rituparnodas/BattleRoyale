// ©Copyright DevDasTour

#include "MainMenu.h"
#include "Components/Button.h"
#include "Engine/Engine.h"
#include "MainMenuPlayerController.h"

bool UMainMenu::Initialize()
{
	bool Success = Super::Initialize();
	if (!Success) return false;

	if (!FindMatchButton) return false;
	FindMatchButton->OnClicked.AddDynamic(this, &UMainMenu::FindAMatch);
	
	return true;
}

void UMainMenu::FindAMatch()
{
	if (!MenuInterface) return;
	MenuInterface->TryJoinOrCreateGame();
}

void UMainMenu::SetMenuInterface(IMenuInterface* MenuToInterface)
{
	this->MenuInterface = MenuToInterface;
}

void UMainMenu::Setup()
{
	AMainMenuPlayerController* PC = Cast<AMainMenuPlayerController>(GetWorld()->GetFirstPlayerController());
	if (PC)
	{
		FInputModeUIOnly InputModeData;
		InputModeData.SetWidgetToFocus(this->TakeWidget());
		InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputModeData);
		PC->bShowMouseCursor = true;
	}
}