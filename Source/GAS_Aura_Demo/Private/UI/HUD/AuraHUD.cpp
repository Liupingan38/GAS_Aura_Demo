// Copyright Liupingan


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController=NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetPlayerControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass,TEXT("Overlay Widget Class is null, please fill BP_auraHUD"));
	checkf(OverlayWidgetControllerClass,TEXT("Overlay Widget Controller Class is null, please fill BP_auraHUD"));
	
	UUserWidget* Widget=CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget=Cast<UAuraUserWidget>(Widget);

	const FWidgetControllerParams WCParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController=GetOverlayWidgetController(WCParams);
	OverlayWidget->SetWidgetController(WidgetController);

	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
}


