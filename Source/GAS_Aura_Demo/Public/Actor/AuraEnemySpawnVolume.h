// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/SaveInterface.h"
#include "AuraEnemySpawnVolume.generated.h"

class AAuraEnemySpawnPoint;
class UBoxComponent;

UCLASS()
class GAS_AURA_DEMO_API AAuraEnemySpawnVolume : public AActor,public ISaveInterface
{
	GENERATED_BODY()
	
public:	
	
	AAuraEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */

	UPROPERTY(SaveGame,BlueprintReadOnly)
	bool bReached;
	
protected:
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere)
	TArray<AAuraEnemySpawnPoint*> SpawnPoints;

private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;

};
