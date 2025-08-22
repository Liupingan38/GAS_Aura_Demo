// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Checkpoint/Checkpoint.h"
#include "MapEntrance.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AMapEntrance : public ACheckpoint
{
	GENERATED_BODY()
public:
	AMapEntrance(const FObjectInitializer& ObjectInitializer);
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UWorld> DestinationMap;

	UPROPERTY(EditAnywhere)
	FName DestinationPlayerStartTag;
protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								 int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	
	/* Highlight Interface */
	virtual void Highlight_Implementation() override;
	/* End Highlight Interface */

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */
};
