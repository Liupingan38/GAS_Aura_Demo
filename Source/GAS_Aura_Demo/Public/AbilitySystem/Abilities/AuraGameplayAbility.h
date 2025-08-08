// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/** 
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="input")
	FGameplayTag StartupInputTag;

	virtual FString GetDescription(int32 AbilityLevel);
	virtual FString GetNextLevelDescription(int32 NextAbilityLevel);

	static FString GetLockedDescription(int32 AbilityLevel);

protected:
	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;
};
