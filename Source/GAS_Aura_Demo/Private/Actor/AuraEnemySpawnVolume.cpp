// Copyright Liupingan


#include "Actor/AuraEnemySpawnVolume.h"

// Sets default values
AAuraEnemySpawnVolume::AAuraEnemySpawnVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAuraEnemySpawnVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAuraEnemySpawnVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

