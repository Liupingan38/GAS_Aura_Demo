// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/SaveGame.h"
#include "LoadScreenSaveGame.generated.h"

class UGameplayAbility;

UENUM(BlueprintType)
enum ESaveSlotStatus
{
	Vacant,
	Taken,
	Entered
};

USTRUCT(BlueprintType)
struct FSavedAbility
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="ClassDefaults")
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityTag=FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityType=FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilitySlot=FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	FGameplayTag AbilityStatus=FGameplayTag();

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	int32 AbilityLevel;
};

inline bool operator==(const FSavedAbility& Left, const FSavedAbility& Right)
{
	return Left.AbilityTag==Right.AbilityTag;
}

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API ULoadScreenSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FString SlotName = FString();

	UPROPERTY()
	int32 SlotIndex = 0;

	UPROPERTY()
	FString PlayerName = FString("Default Name");

	UPROPERTY()
	FString MapName = FString();

	UPROPERTY()
	FName PlayerStartTag;

	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> SlotStatus = Vacant;

	/* 玩家信息 */
	UPROPERTY()
	int32 PlayerLevel = 1;
	UPROPERTY()
	int32 XP = 0;
	UPROPERTY()
	int32 AttributePoints = 0;
	UPROPERTY()
	int32 SpellPoints = 0;
	UPROPERTY()
	bool bFirstTimeLoadIn = true;
	UPROPERTY()
	float Strength = 0;
	UPROPERTY()
	float Intelligence = 0;
	UPROPERTY()
	float Resilience = 0;
	UPROPERTY()
	float Vigor = 0;

	/* 技能 */

	UPROPERTY()
	TArray<FSavedAbility> SavedAbilities;
	
};
