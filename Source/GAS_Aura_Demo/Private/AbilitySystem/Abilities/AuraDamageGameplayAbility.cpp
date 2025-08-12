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
	                                                                          UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
		                                                                          Target));
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.SourceAbilitySystemComponent = GetAbilitySystemComponentFromActorInfo();
	Params.TargetAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffDuration = DebuffDuration;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackMagnitude = KnockbackMagnitude;
	Params.KnockbackChance = KnockbackChance;
	if (IsValid(TargetActor))
	{
		FRotator ToTargetRotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		ToTargetRotation.Pitch = 45.f;
		const FVector ToTarget = ToTargetRotation.Vector();
		Params.DeathImpulse = Params.DeathImpulseMagnitude * ToTarget;
		Params.KnockbackImpulse = Params.KnockbackMagnitude * ToTarget;
	}

	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const 
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}

FTaggedMontage UAuraDamageGameplayAbility::GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontageArray) const
{
	if (TaggedMontageArray.Num() == 0) return FTaggedMontage();
	const int32 Selection = FMath::RandRange(0, TaggedMontageArray.Num() - 1);
	return TaggedMontageArray[Selection];
}
