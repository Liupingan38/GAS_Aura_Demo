// Copyright Liupingan


#include "Game/AuraGameModeBase.h"

#include "EngineUtils.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadScreenSaveGame.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "GAS_Aura_Demo/AuraLogChannels.h"
#include "Interaction/SaveInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void AAuraGameModeBase::SaveWorldState(UWorld* World, const FString& DestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if (ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		if (DestinationMapAssetName!=FString(""))
		{
			SaveGame->MapAssetName = DestinationMapAssetName;
			SaveGame->MapName=GetMapNameFromMapAssetName(DestinationMapAssetName);
		}
		if (!SaveGame->HasMap(WorldName))
		{
			FSavedMap NewSaveMap;
			NewSaveMap.MapName = WorldName;
			SaveGame->SavedMaps.Add(NewSaveMap);
		}
		FSavedMap SavedMap = SaveGame->GetSavedMapFromMapName(WorldName);
		SavedMap.SavedActors.Empty();

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;

			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			FSavedActor SavedActor;
			SavedActor.ActorName = Actor->GetFName();
			SavedActor.ActorTransform = Actor->GetActorTransform();

			FMemoryWriter MemoryWriter(SavedActor.Bytes);
			FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
			Archive.ArIsSaveGame = true;

			Actor->Serialize(Archive);

			SavedMap.SavedActors.AddUnique(SavedActor);
		}
		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapName == WorldName)
			{
				MapToReplace = SavedMap;
			}
		}
		UGameplayStatics::SaveGameToSlot(SaveGame, AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
	}
}

void AAuraGameModeBase::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	UAuraGameInstance* AuraGI = Cast<UAuraGameInstance>(GetGameInstance());
	check(AuraGI);

	if (UGameplayStatics::DoesSaveGameExist(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex))
	{
		ULoadScreenSaveGame* SaveGame = GetSaveSlotData(AuraGI->LoadSlotName, AuraGI->LoadSlotIndex);
		if (SaveGame == nullptr)
		{
			UE_LOG(LogAura, Error, TEXT("Fail to load SaveGame!"));
			return;
		}

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (!IsValid(Actor) || !Actor->Implements<USaveInterface>()) continue;

			FSavedMap SavedMap = SaveGame->GetSavedMapFromMapName(WorldName);
			for (const FSavedActor& SavedActor : SavedMap.SavedActors)
			{
				if (Actor->GetFName() == SavedActor.ActorName)
				{
					if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
					{
						Actor->SetActorTransform(SavedActor.ActorTransform);
					}
					FMemoryReader MemoryReader(SavedActor.Bytes);
					FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
					Archive.ArIsSaveGame = true;
					Actor->Serialize(Archive);
					ISaveInterface::Execute_LoadActor(Actor);
				}
			}
		}
	}
}

void AAuraGameModeBase::SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex)
{
	DeleteSlotData(LoadSlot->GetLoadSlotName(), SlotIndex);
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	ULoadScreenSaveGame* LoadScreenSaveGame = Cast<ULoadScreenSaveGame>(SaveGameObject);
	LoadScreenSaveGame->PlayerName = LoadSlot->GetPlayerName();
	LoadScreenSaveGame->SlotStatus = Entered;
	LoadScreenSaveGame->MapName = LoadSlot->GetMapName();
	LoadScreenSaveGame->MapAssetName=LoadSlot->MapAssetName;
	LoadScreenSaveGame->PlayerStartTag = LoadSlot->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(LoadScreenSaveGame, LoadSlot->GetLoadSlotName(), SlotIndex);
}

void AAuraGameModeBase::DeleteSlotData(const FString& SlotName, int32 SlotIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, SlotIndex);
	}
}

ULoadScreenSaveGame* AAuraGameModeBase::GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const
{
	USaveGame* SaveGameObject;
	if (UGameplayStatics::DoesSaveGameExist(SlotName, SlotIndex))
	{
		SaveGameObject = UGameplayStatics::LoadGameFromSlot(SlotName, SlotIndex);
	}
	else
	{
		SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadScreenSaveGameClass);
	}
	return Cast<ULoadScreenSaveGame>(SaveGameObject);
}

ULoadScreenSaveGame* AAuraGameModeBase::RetrieveInGameSaveData()
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString& InGameSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameSlotIndex = AuraGameInstance->LoadSlotIndex;

	return GetSaveSlotData(InGameSlotName, InGameSlotIndex);
}

void AAuraGameModeBase::SaveInGameProgressData(ULoadScreenSaveGame* SaveGame)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	const FString& InGameSlotName = AuraGameInstance->LoadSlotName;
	const int32 InGameSlotIndex = AuraGameInstance->LoadSlotIndex;
	AuraGameInstance->PlayerStartTag = SaveGame->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveGame, InGameSlotName, InGameSlotIndex);
}

void AAuraGameModeBase::TravelToMap(UMVVM_LoadSlot* LoadSlot)
{
	const FString SlotName = LoadSlot->GetLoadSlotName();
	const int32 SlotIndex = LoadSlot->SlotIndex;

	UGameplayStatics::OpenLevelBySoftObjectPtr(LoadSlot, Maps.FindChecked(LoadSlot->GetMapName()));
}

FString AAuraGameModeBase::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	for(auto& Map: Maps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetFName()==MapAssetName)
		{
			return Map.Key;
		}
	}
	return FString();
}

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());
	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AllActors);
	if (AllActors.Num() > 0)
	{
		AActor* SelectedActor = AllActors[0];
		for (AActor* Actor : AllActors)
		{
			if (APlayerStart* PlayerStart = Cast<APlayerStart>(Actor))
			{
				if (PlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					SelectedActor = PlayerStart;
					break;
				}
			}
		}
		return SelectedActor;
	}
	return nullptr;
}

void AAuraGameModeBase::PlayerDied(const ACharacter* DeadCharacter)
{
	const ULoadScreenSaveGame* SaveGame=RetrieveInGameSaveData();
	if (!IsValid(SaveGame)) return;

	UGameplayStatics::OpenLevel(DeadCharacter,FName(SaveGame->MapAssetName));
}

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	Maps.Add(DefaultMapName, DefaultMap);
}
