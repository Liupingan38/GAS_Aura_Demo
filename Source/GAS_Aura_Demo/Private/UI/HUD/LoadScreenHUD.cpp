// Copyright Liupingan


#include "UI/HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadScreenViewModel=NewObject<UMVVM_LoadScreen>(this,LoadScreenViewModelClass);
	LoadScreenViewModel->InitializeLoadSlot();
	
	LoadScreenWidget=CreateWidget<ULoadScreenWidget>(GetWorld(),LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();

	APlayerController* PC = GetOwningPlayerController();
	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(LoadScreenWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	PC->SetInputMode(InputMode);
	PC->SetShowMouseCursor(true);
	
	LoadScreenWidget->BlueprintInitializeWidget();
	
	LoadScreenViewModel->LoadData();
}
