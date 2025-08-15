// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

FString UAuraBeamSpell::GetDescription(int32 AbilityLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(AbilityLevel);
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	return FString::Printf(TEXT(
		//标题
		"<LightingTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n"
		//冷却时间
		"<Small>冷却时间：</><Time>%.1f</>\n"
		//法术消耗
		"<Small>法术消耗：</><ManaCost>%.1f</>\n\n"
		//伤害
		//额外链接个数
		"<Default>发射 </><Num>1</><Default> 条闪电光束，对命中的首个目标每 </><Time>0.5</><Default> 秒造成 </><Lighting>%d</><Default> 点</><Lighting>电击伤害</>"
		"<Default>，并链接附近最多  </><Num>%d</><Default> 个敌人，对其造成 </><Percent>50%%</><Default> 首要目标所受到的</><Lighting>电击伤害</>"
		"<Default>，每次电击伤害有 </><Percent>20%%</><Default> 概率造成 </><Lighting>眩晕</><Default> 状态。</>"),
						   L"闪电链",
						   AbilityLevel,
						   Cooldown,
						   FMath::Abs(ManaCost),
						   ScaledDamage,
						   FMath::Min(AbilityLevel-1, MaxNumAdditionalTargets)
	);
}

FString UAuraBeamSpell::GetNextLevelDescription(int32 NextAbilityLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(NextAbilityLevel);
	const float ManaCost = GetManaCost(NextAbilityLevel);
	const float Cooldown = GetCooldown(NextAbilityLevel);
	return FString::Printf(TEXT(
		//标题
		"<LightingTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n"
		//冷却时间
		"<Small>冷却时间：</><Time>%.1f</>\n"
		//法术消耗
		"<Small>法术消耗：</><ManaCost>%.1f</>\n\n"
		//伤害
		//额外链接个数
		"<Default>发射 </><Num>1</><Default> 条闪电光束，对命中的首个目标每 </><Time>0.5</><Default> 秒造成 </><Lighting>%d</><Default> 点</><Lighting>电击伤害</>"
		"<Default>，并链接附近最多  </><Num>%d</><Default> 个敌人，对其造成 </><Percent>50%%</><Default> 首要目标所受到的</><Lighting>电击伤害</>"
		"<Default>，每次电击伤害有 </><Percent>20%%</><Default> 概率造成 </><Lighting>眩晕</><Default> 状态。</>"),
						   L"下一级：",
						   NextAbilityLevel,
						   Cooldown,
						   FMath::Abs(ManaCost),
						   ScaledDamage,
						   FMath::Min(NextAbilityLevel-1, MaxNumAdditionalTargets)
	);
}

void UAuraBeamSpell::StoreMouseHitDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitActor = HitResult.GetActor();
		MouseHitLocation = HitResult.ImpactPoint;
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors,
	                                                     ActorsToIgnore, 500.f, MouseHitActor->GetActorLocation());
	const int32 NumAdditionalTargets = FMath::Min(MaxNumAdditionalTargets, GetAbilityLevel()-1);

	UAuraAbilitySystemLibrary::GetClosestTargets(MouseHitActor->GetActorLocation(), NumAdditionalTargets, OverlappingActors,
	                                             OutAdditionalTargets);

	for (AActor* AdditionalActor:OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface=Cast<ICombatInterface>(AdditionalActor))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (const USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f,
			                                        TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (HitResult.bBlockingHit)
			{
				MouseHitActor = HitResult.GetActor();
				MouseHitLocation = HitResult.ImpactPoint;
			}
		}
	}
	if (ICombatInterface* CombatInterface=Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::FirstTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::FirstTargetDied);
		}
	}
}
