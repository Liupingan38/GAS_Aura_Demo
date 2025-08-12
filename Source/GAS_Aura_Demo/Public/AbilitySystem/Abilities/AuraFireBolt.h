// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraProjectileSpell.h"
#include "AuraFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UAuraFireBolt : public UAuraProjectileSpell
{
	GENERATED_BODY()

public:
	virtual FString GetDescription(int32 AbilityLevel) override;
	virtual FString GetNextLevelDescription(int32 NextAbilityLevel) override;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget,
	                      bool bOverridePitch = false, float PitchOverride = 0.f);

protected:
	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	int32 MaxNumProjectiles = 5;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float ProjectileSpreadAngle = 90.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float MinHomingAcceleration = 6000.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	float MaxHomingAcceleration = 9000.f;

	UPROPERTY(EditDefaultsOnly, Category="FireBolt")
	bool bHoming = true;
};
