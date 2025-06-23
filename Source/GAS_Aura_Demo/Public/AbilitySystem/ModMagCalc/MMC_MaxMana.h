// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxMana();
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
