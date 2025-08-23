// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Engine/TargetPoint.h"
#include "AuraEnemySpawnPoint.generated.h"

enum class ECharacterClass : uint8;
class AAuraEnemy;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraEnemySpawnPoint : public ATargetPoint
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void SpawnEnemy();

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Enemy Spawner")
	TSubclassOf<AAuraEnemy> EnemyClass;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Enemy Spawner")
	int32 EnemyLevel=1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Enemy Spawner")
	ECharacterClass CharacterClass=ECharacterClass::Warrior;
	
};
