// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "GAS_Aura_Demo/GAS_Aura_Demo.h"
#include "Interaction/HighlightInterface.h"
#include "Interaction/SaveInterface.h"
#include "Checkpoint.generated.h"

class USphereComponent;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API ACheckpoint : public APlayerStart,public ISaveInterface,public IHighlightInterface
{
	GENERATED_BODY()

public:
	ACheckpoint(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(SaveGame,BlueprintReadWrite)
	bool bReached=false;

	UPROPERTY(EditAnywhere)
	bool bBindOverlayCallback=true;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                             int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/* Save Interface */
	virtual bool ShouldLoadTransform_Implementation() override{return false;}
	virtual void LoadActor_Implementation() override;
	/* End Save Interface */

	/* Highlight Interface */
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	/* End Highlight Interface */

	UFUNCTION(BlueprintImplementableEvent)
	void HandleGlowDynamicMaterial(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintCallable)
	void CheckpointReached();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilOverride=CUSTOM_DEPTH_Blue;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
};
