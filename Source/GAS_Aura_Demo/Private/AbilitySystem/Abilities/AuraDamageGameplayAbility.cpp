// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UAuraDamageGameplayAbility::CauseDamage(AActor* Target)
{
	const FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	
	const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	//设置DamageEffectClass 中SetByCaller 
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageType, ScaleDamage); 

	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),
	UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Target));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject=GetAvatarActorFromActorInfo();
	Params.SourceAbilitySystemComponent=GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.DamageGameplayEffectClass=DamageEffectClass;
	Params.BaseDamage=Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel=GetAbilityLevel();
	Params.DamageType=DamageType;
	Params.DebuffChance=DebuffChance;
	Params.DebuffDamage=DebuffDamage;
	Params.DebuffDuration=DebuffDuration;
	Params.DebuffFrequency=DebuffFrequency;
	return Params;
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontageArray) const
{
	if (TaggedMontageArray.Num() == 0) return FTaggedMontage();
	const int32 Selection=FMath::RandRange(0,TaggedMontageArray.Num()-1);
	return TaggedMontageArray[Selection];
}

