// Copyright Liupingan


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Game/AuraGameInstance.h"
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

void UMVVM_LoadScreen::NewGameButtonPressed(int32 InSlotIndex)
{
	LoadSlotMap[InSlotIndex]->SlotStatus=Taken;
	LoadSlotMap[InSlotIndex]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 InSlotIndex, const FString& EnteredName)
{
	AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode))
	{
		GEngine->AddOnScreenDebugMessage(1,15.f,FColor::Red,FString("Please switch to Single Player"));
		return;
	}
	LoadSlotMap[InSlotIndex]->SetPlayerName(EnteredName);
	LoadSlotMap[InSlotIndex]->SlotStatus=Entered;
	LoadSlotMap[InSlotIndex]->SetMapName(AuraGameMode->DefaultMapName);
	LoadSlotMap[InSlotIndex]->SetPlayerLevel(1);
	LoadSlotMap[InSlotIndex]->PlayerStartTag=AuraGameMode->DefaultPlayerStartTag;
	LoadSlotMap[InSlotIndex]->MapAssetName=AuraGameMode->DefaultMap.ToSoftObjectPath().GetAssetName();
	
	AuraGameMode->SaveSlotData(LoadSlotMap[InSlotIndex], InSlotIndex);

	LoadSlotMap[InSlotIndex]->SetWidgetSwitcherIndex.Broadcast(2);

	UAuraGameInstance* AuraGameInstance=Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->LoadSlotName=LoadSlotMap[InSlotIndex]->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex=LoadSlotMap[InSlotIndex]->SlotIndex;
	AuraGameInstance->PlayerStartTag=AuraGameMode->DefaultPlayerStartTag;
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 InSlotIndex)
{
	SlotSelected.Broadcast();
	SelectedSlot=LoadSlotMap[InSlotIndex];
	for (const TPair<int, UMVVM_LoadSlot*>& Pair:LoadSlotMap)
	{
		const int32 SlotIndex=Pair.Key;
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
	UAuraGameInstance* AuraGameInstance=Cast<UAuraGameInstance>(AuraGameMode->GetGameInstance());
	AuraGameInstance->PlayerStartTag=SelectedSlot->PlayerStartTag;
	AuraGameInstance->LoadSlotName=SelectedSlot->GetLoadSlotName();
	AuraGameInstance->LoadSlotIndex=SelectedSlot->SlotIndex;
	
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
	const AAuraGameModeBase* AuraGameMode=Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this));
	if (!IsValid(AuraGameMode)) return; //客户端无法加载
	for (const TPair<int, UMVVM_LoadSlot*>& Pair:LoadSlotMap)
	{
		const int32 SlotIndex=Pair.Key;
		UMVVM_LoadSlot* LoadSlotViewModel=Pair.Value;
		const ULoadScreenSaveGame* SaveGame=AuraGameMode->GetSaveSlotData(LoadSlotViewModel->GetLoadSlotName(), SlotIndex);
		
		LoadSlotViewModel->SlotStatus=SaveGame->SlotStatus;
		LoadSlotViewModel->SetPlayerName(SaveGame->PlayerName);
		LoadSlotViewModel->SetMapName(SaveGame->MapName);
		LoadSlotViewModel->SetPlayerLevel(SaveGame->PlayerLevel);
		LoadSlotViewModel->PlayerStartTag=SaveGame->PlayerStartTag;
		LoadSlotViewModel->InitializeSlot();
	}
}

void UMVVM_LoadScreen::SetLoadScreenName(FString InLoadScreenName)
{
	UE_MVVM_SET_PROPERTY_VALUE(LoadScreenName, InLoadScreenName);
}
