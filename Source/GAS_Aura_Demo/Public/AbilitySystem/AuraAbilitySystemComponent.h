// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer&)
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void AbilityActorInfoSet();

	FEffectAssetTags EffectAssetTags;

	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:
	UFUNCTION(Client, reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle) ;
};
