// Copyright Liupingan


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::InitializeLoadSlot()
{
	LoadSlot_0=NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotMap.Add(0,LoadSlot_0);
	LoadSlot_0->SetLoadSlotName(FString("LoadSlot_0"));
	LoadSlot_0->SlotIndex=0;
	
	LoadSlot_1=NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotMap.Add(1,LoadSlot_1);
	LoadSlot_1->SetLoadSlotName(FString("LoadSlot_1"));
	LoadSlot_1->SlotIndex=1;
	
	LoadSlot_2=NewObject<UMVVM_LoadSlot>(this,LoadSlotViewModelClass);
	LoadSlotMap.Add(2,LoadSlot_2);
	LoadSlot_2->SetLoadSlotName(FString("LoadSlot_2"));
	LoadSlot_2->SlotIndex=2;
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotByIndex(int32 Index) const
{
	return LoadSlotMap.FindChecked(Index);
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 SlotIndex)
{
	LoadSlotMap[SlotIndex]->SlotStatus=Taken;
	LoadSlotMap[SlotIndex]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 SlotIndex, const FString& EnteredName)
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	LoadSlotMap[SlotIndex]->SetPlayerName(EnteredName);
	LoadSlotMap[SlotIndex]->SlotStatus=Entered;
	LoadSlotMap[SlotIndex]->SetMapName(AuraGameMode->DefaultMapName);
	
	AuraGameMode->SaveSlotData(LoadSlotMap[SlotIndex], SlotIndex);

	LoadSlotMap[SlotIndex]->SetWidgetSwitcherIndex.Broadcast(2);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 InSlotIndex)
{
	SlotSelected.Broadcast();
	SelectedSlot=LoadSlotMap[InSlotIndex];
	for (const TPair<int, UMVVM_LoadSlot*>& Pair:LoadSlotMap)
	{
		int32 SlotIndex=Pair.Key;
		const UMVVM_LoadSlot* LoadSlotViewModel=Pair.Value;
		if (SlotIndex==InSlotIndex)
		{
			LoadSlotViewModel->EnableSelectSlotButton.Broadcast(false);
		}else
		{
			LoadSlotViewModel->EnableSelectSlotButton.Broadcast(true);
		}
	}
}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (IsValid(AuraGameMode)&&IsValid(SelectedSlot))
	{
		AuraGameMode->TravelToMap(LoadSlotMap[SelectedSlot->SlotIndex]);
	}
}

void UMVVM_LoadScreen::RUSDeleteButtonPressed()
{
	if (IsValid(SelectedSlot))
	{
		AAuraGameModeBase::DeleteSlotData(SelectedSlot->GetLoadSlotName(),SelectedSlot->SlotIndex);
		SelectedSlot->SlotStatus=Vacant;
		SelectedSlot->InitializeSlot();
		SelectedSlot->EnableSelectSlotButton.Broadcast(true);
	}
	
}

void UMVVM_LoadScreen::LoadData()
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	for (const TPair<int, UMVVM_LoadSlot*>& Pair:LoadSlotMap)
	{
		int32 SlotIndex=Pair.Key;
		UMVVM_LoadSlot* LoadSlotViewModel=Pair.Value;
		const ULoadScreenSaveGame* LoadScreenSaveGame=AuraGameMode->GetSaveSlotData(LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
		
		LoadSlotViewModel->SlotStatus=LoadScreenSaveGame->SlotStatus;
		LoadSlotViewModel->SetPlayerName(LoadScreenSaveGame->PlayerName);
		LoadSlotViewModel->SetMapName(LoadScreenSaveGame->MapName);
		LoadSlotViewModel->InitializeSlot();
	}
}

void UMVVM_LoadScreen::SetLoadScreenName(FString InLoadScreenName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadScreenName, InLoadScreenName);
}
