// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/PassiveSpell/AuraPassiveAbility.h"
#include "AuraHealthSiphon.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UAuraHealthSiphon : public UAuraPassiveAbility
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 AbilityLevel) override;
	virtual FString GetNextLevelDescription(int32 NextAbilityLevel) override;
};
