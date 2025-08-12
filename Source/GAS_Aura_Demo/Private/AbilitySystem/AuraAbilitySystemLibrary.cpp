// Copyright Liupingan


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "Game/AuraGameModeBase.h"
#include "Interaction/CombatInterface.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutWCParams,
                                                           AAuraHUD*& OutAuraHUD)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutAuraHUD = Cast<AAuraHUD>(PC->GetHUD());
		if (OutAuraHUD)
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutWCParams = FWidgetControllerParams(PC, PS, ASC, AS);
			return true;
		}
	}
	return false;
}

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD=nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD)){
		return AuraHUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD=nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD)){
		return AuraHUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	AAuraHUD* AuraHUD=nullptr;
	if (MakeWidgetControllerParams(WorldContextObject, WCParams, AuraHUD)){
		return AuraHUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                            ECharacterClass CharacterClass,
                                                            float Level,
                                                            UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetCharacterClassInfo(CharacterClass);
	AActor* AvatarActor = ASC->GetAvatarActor();

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, ECharacterClass CharacterClass,
                                                     UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;

	for (auto AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		//通用能力如 受击反应 不受等级影响
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetCharacterClassInfo(CharacterClass);
	for (const auto& AbilityClass : ClassDefaultInfo.ClassStartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(
				AbilityClass, ICombatInterface::Execute_GetCombatCharacterLevel(ASC->GetAvatarActor()));
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

int32 UAuraAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass,
                                                             int32 CharacterLevel)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return 0;

	const FCharacterClassDefaultInfo& Info = CharacterClassInfo->GetCharacterClassInfo(CharacterClass);
	const float XP = Info.XPReward.GetValueAtLevel(CharacterLevel);
	return static_cast<int32>(XP);
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& Params)
{
	const FAuraGameplayTags& Tags=FAuraGameplayTags::Get();
	const UAbilitySystemComponent* SourceASC=Params.SourceAbilitySystemComponent;
	
	FGameplayEffectContextHandle ContextHandle=SourceASC->MakeEffectContext();
	ContextHandle.AddSourceObject(SourceASC->GetAvatarActor());
	SetDeathImpulse(ContextHandle, Params.DeathImpulse);
	SetKnockbackImpulse(ContextHandle, Params.KnockbackImpulse);
	
	const FGameplayEffectSpecHandle SpecHandle=SourceASC->MakeOutgoingSpec(
		Params.DamageGameplayEffectClass,Params.AbilityLevel,ContextHandle);
	
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Params.DamageType,Params.BaseDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Tags.Debuff_Chance,Params.DebuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Tags.Debuff_Damage,Params.DebuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Tags.Debuff_Duration,Params.DebuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle,Tags.Debuff_Frequency,Params.DebuffFrequency);
	
	Params.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	return ContextHandle;
}

TArray<FVector> UAuraAbilitySystemLibrary::EvenlySpreadVectors(const FVector& Forward, float SpreadAngle, int32 Nums, const FVector& Axis)
{
	TArray<FVector> Result;
	const FVector LeftDirection = Forward.RotateAngleAxis(-SpreadAngle / 2.f, Axis);
	if (Nums > 1)
	{
		const float DeltaAngle = SpreadAngle / (Nums - 1);
		for (int32 i = 0; i < Nums; i++)
		{
			FVector CurDirection = LeftDirection.RotateAngleAxis(DeltaAngle * i, Axis);
			Result.Add(CurDirection);
		}
	}
	else
	{
		Result.Add(Forward);
	}
	return Result;
}

TArray<FRotator> UAuraAbilitySystemLibrary::EvenlySpreadRotators(const FVector& Forward, float SpreadAngle, int32 Nums, const FVector& Axis)
{
	TArray<FRotator> Result;
	const FVector LeftDirection = Forward.RotateAngleAxis(-SpreadAngle / 2.f, Axis);
	if (Nums > 1)
	{
		const float DeltaAngle = SpreadAngle / (Nums - 1);
		for (int32 i = 0; i < Nums; i++)
		{
			FVector CurDirection = LeftDirection.RotateAngleAxis(DeltaAngle * i, Axis);
			Result.Add(CurDirection.Rotation());
		}
	}
	else
	{
		Result.Add(Forward.Rotation());
	}
	return Result;
}


UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return AuraGameModeBase ? AuraGameModeBase->CharacterClassInfo : nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const AAuraGameModeBase* AuraGameModeBase = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return AuraGameModeBase ? AuraGameModeBase->AbilityInfo : nullptr;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsSuccessfulDebuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->IsSuccessfulDebuff();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetDebuffDuration();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetDebuffFrequency();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (Context->GetDamageType().IsValid())
		{
			return *Context->GetDamageType();
		}
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockbackImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FAuraGameplayEffectContext* Context = static_cast<const FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return Context->GetKnockbackImpulse();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsBlockedHit)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, const bool bInIsCriticalHit)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UAuraAbilitySystemLibrary::SetIsSuccessfulDebuff(FGameplayEffectContextHandle& EffectContextHandle, const bool bInDebuff)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetIsSuccessfulDebuff(bInDebuff);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& EffectContextHandle, const float InDamage)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetDebuffDamage(InDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& EffectContextHandle, const float InDuration)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetDebuffDuration(InDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& EffectContextHandle, const float InFrequency)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetDebuffFrequency(InFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& EffectContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
		Context->SetDamageType(DamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockbackImpulse(FGameplayEffectContextHandle& EffectContextHandle, const FVector& InKnockbackImpulse)
{
	if (FAuraGameplayEffectContext* Context = static_cast<FAuraGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		Context->SetKnockbackImpulse(InKnockbackImpulse);
	}
}

void UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors,
                                                          const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity,
		                                FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects),
		                                FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDie(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(ICombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

void UAuraAbilitySystemLibrary::GetClosestTargets(const FVector& Origin, int32 NumTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets)
{
	OutClosestTargets.Empty();
	if (Actors.Num() <= NumTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	struct FDistanceSquaredActor
	{
		float SquaredDistance;
		AActor* Actor;

		bool operator<(const FDistanceSquaredActor& Other) const
		{
			return SquaredDistance > Other.SquaredDistance;
		}
	};

	TArray<FDistanceSquaredActor> Heap;
	Heap.Reserve(NumTargets);
	for (AActor* Actor : Actors)
	{
		if (!Actor) continue;
		
		const float CurDistanceSquared=FVector::DistSquared(Origin, Actor->GetActorLocation());
		FDistanceSquaredActor CurActor={CurDistanceSquared, Actor};
		if (Heap.Num() < NumTargets)
		{
			Heap.Add(CurActor);
			if (Heap.Num() == NumTargets)
			{
				Algo::Heapify(Heap);
			}
		}else if (CurDistanceSquared<Heap[0].SquaredDistance)
		{
			Heap[0]=CurActor;
			Algo::Heapify(Heap);
		}
	}
	for (auto& [SquaredDistance, Actor] : Heap)
	{
		OutClosestTargets.Add(Actor);
	}
	
}

bool UAuraAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothPlayer = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothEnemy = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bIsFriend = bBothPlayer || bBothEnemy;
	return !bIsFriend;
}
