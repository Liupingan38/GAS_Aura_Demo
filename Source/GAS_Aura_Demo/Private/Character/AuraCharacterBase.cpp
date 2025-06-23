// Copyright Liupingan


#include "Character/AuraCharacterBase.h"


AAuraCharacterBase::AAuraCharacterBase()
{
 	
	PrimaryActorTick.bCanEverTick = false;
	Weapon=CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& EffectClass, float Level) const
{
	check(EffectClass);
	UAbilitySystemComponent* ASC=GetAbilitySystemComponent();
	check(ASC);
	FGameplayEffectContextHandle ContextHandle=ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle=ASC->MakeOutgoingSpec(EffectClass,Level,ContextHandle);
	ASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(),ASC);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributesEffectClass,1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributesEffectClass,1.f);
	ApplyEffectToSelf(DefaultVitalAttributesEffectClass,1.f);
}



