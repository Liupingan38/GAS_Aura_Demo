// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags,const FGameplayTagContainer&)
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilityGiven,UAuraAbilitySystemComponent*)
DECLARE_DELEGATE_OneParam(FForEachAbility,const FGameplayAbilitySpec&)

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
	FAbilityGiven AbilityGivenDelegate;
	bool bStartupAbilitiesGiven=false;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& Abilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ForEachAbility(const FForEachAbility& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	
protected:

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
		const FGameplayEffectSpec& EffectSpec,FActiveGameplayEffectHandle ActiveEffectHandle) ;
};
