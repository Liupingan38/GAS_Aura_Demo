// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "Interaction/CombatInterface.h"
#include "AuraDamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void CauseDamage(AActor* Target);

	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor=nullptr)const;

protected:

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float DebuffChance=20.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float DebuffDamage=2.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float DebuffDuration=5.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float DebuffFrequency=1.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float DeathImpulseMagnitude=5000.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float KnockbackMagnitude=1000.f;

	UPROPERTY(EditDefaultsOnly,Category="Damage" )
	float KnockbackChance=0.f;
	
	UFUNCTION(Blueprintpure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontageArray) const;

};
