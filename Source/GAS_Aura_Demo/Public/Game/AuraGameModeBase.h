// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "LoadScreenSaveGame.h"
#include "GameFramework/GameModeBase.h"
#include "AuraGameModeBase.generated.h"

class ULootTiers;
class USaveGame;
class UCharacterClassInfo;
class UMVVM_LoadSlot;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category="AbilityInfo")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, Category="Loot Tiers")
	TObjectPtr<ULootTiers> LootTiers;

	void SaveWorldState(UWorld* World, const FString& DestinationMapAssetName = FString("")) const;
	void LoadWorldState(UWorld* World) const;
	void SaveSlotData(UMVVM_LoadSlot* LoadSlot, int32 SlotIndex);
	static void DeleteSlotData(const FString& SlotName, int32 SlotIndex);
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;
	ULoadScreenSaveGame* RetrieveInGameSaveData();
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveGame);

	void TravelToMap(UMVVM_LoadSlot* LoadSlot);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	void PlayerDied(const ACharacter* DeadCharacter);

protected:
	virtual void BeginPlay() override;
};
