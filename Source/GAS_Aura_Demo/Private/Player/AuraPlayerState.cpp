// Copyright Liupingan


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);


	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAuraPlayerState, Level)
	DOREPLIFETIME(AAuraPlayerState, XP)
	DOREPLIFETIME(AAuraPlayerState, AttributePoint)
	DOREPLIFETIME(AAuraPlayerState, SpellPoint)

}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level,true);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level,false);
}


void AAuraPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::AddToAttributePoint(int32 InAttributePoint)
{
	AttributePoint += InAttributePoint;
	OnAttributePointChangedDelegate.Broadcast(AttributePoint);
}

void AAuraPlayerState::AddToSpellPoint(int32 InSpellPoint)
{
	SpellPoint += InSpellPoint;
	OnSpellPointChangedDelegate.Broadcast(SpellPoint);
}

void AAuraPlayerState::SetAttributePoint(int32 InAttributePoint)
{
	AttributePoint = InAttributePoint;
	OnAttributePointChangedDelegate.Broadcast(AttributePoint);
}

void AAuraPlayerState::SetSpellPoint(int32 InSpellPoint)
{
	SpellPoint = InSpellPoint;
	OnSpellPointChangedDelegate.Broadcast(SpellPoint);
}

void AAuraPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangedDelegate.Broadcast(Level,true);
}

void AAuraPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangedDelegate.Broadcast(XP);
}

void AAuraPlayerState::OnRep_AttributePoint(int32 OldAttributePoint) const
{
	OnAttributePointChangedDelegate.Broadcast(AttributePoint);
}

void AAuraPlayerState::OnRep_SpellPoint(int32 OldSpellPoint) const
{
	OnSpellPointChangedDelegate.Broadcast(SpellPoint);
}
