// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	Taken,
	Entered
};

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY()
	FString SlotName=FString();

	UPROPERTY()
	int32 SlotIndex=0;

	UPROPERTY()
	FString PlayerName=FString("Default Name");

	UPROPERTY()
	FString MapName=FString();

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus=Vacant;
};
