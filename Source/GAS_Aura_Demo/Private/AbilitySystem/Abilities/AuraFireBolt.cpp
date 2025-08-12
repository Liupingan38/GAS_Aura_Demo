// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"


FString UAuraFireBolt::GetDescription(int32 AbilityLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(AbilityLevel);
	const float ManaCost = GetManaCost(AbilityLevel);
	const float Cooldown = GetCooldown(AbilityLevel);
	return FString::Printf(TEXT(
		//标题
		"<Title>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n"
		//冷却时间
		"<Small>冷却时间：</><Time>%.1f</>\n"
		//法术消耗
		"<Small>法术消耗：</><ManaCost>%.1f</>\n\n"
		//个数
		//伤害
		"<Default>发射 </><Num>%d</><Default> 枚火焰弹，对命中目标造成 </><Fire>%d</><Default> 点</><Fire>火焰伤害</>"
		"<Default>，并且每一枚火焰弹有 </><Percent>20%%</><Default> 概率造成 </><Fire>点燃</><Default> 状态。</>"),
	                       L"火球术",
	                       AbilityLevel,
	                       Cooldown,
	                       FMath::Abs(ManaCost),
	                       FMath::Min(AbilityLevel, MaxNumProjectiles),
	                       ScaledDamage
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 NextAbilityLevel)
{
	const int32 ScaledDamage = Damage.GetValueAtLevel(NextAbilityLevel);
	const float ManaCost = GetManaCost(NextAbilityLevel);
	const float Cooldown = GetCooldown(NextAbilityLevel);
	return FString::Printf(TEXT(
		//标题
		"<Title>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n"
		//冷却时间
		"<Small>冷却时间：</><Time>%.1f</>\n"
		//法术消耗
		"<Small>法术消耗：</><ManaCost>%.1f</>\n\n"
		//个数
		//伤害
		"<Default>发射 </><Num>%d</><Default> 枚火焰弹，对命中目标造成 </><Fire>%d</><Default> 点</><Fire>火焰伤害</>"
		"<Default>，并且每一枚火焰弹有 </><Percent>20%%</><Default> 概率造成 </><Fire>点燃</><Default> 状态。</>"),
	                       L"下一级：",
	                       NextAbilityLevel,
	                       Cooldown,
	                       FMath::Abs(ManaCost),
	                       FMath::Min(NextAbilityLevel, MaxNumProjectiles),
	                       ScaledDamage
	);
}

void UAuraFireBolt::SpawnProjectiles(const FVector& ProjectileTargetLocation, const FGameplayTag& SocketTag, AActor* HomingTarget,
                                     bool bOverridePitch, float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer) return;

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(), SocketTag);
	FRotator ProjectileRotation = (ProjectileTargetLocation - SocketLocation).Rotation();

	if (bOverridePitch) ProjectileRotation.Pitch = PitchOverride;

	const FVector Forward = ProjectileRotation.Vector();
	int32 EffectiveNumProjectiles = FMath::Min(NumProjectiles, GetAbilityLevel());
	TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpreadRotators(Forward, ProjectileSpreadAngle, EffectiveNumProjectiles);

	for (const FRotator& CurRotation : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(CurRotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

		if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
		{
			Projectile->ProjectileMovementComponent->HomingTargetComponent = HomingTarget->GetRootComponent();
		}
		else
		{
			Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
			Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileTargetLocation);
			Projectile->ProjectileMovementComponent->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
		}
		Projectile->ProjectileMovementComponent->HomingAccelerationMagnitude = FMath::RandRange(MinHomingAcceleration, MaxHomingAcceleration);
		Projectile->ProjectileMovementComponent->bIsHomingProjectile = bHoming;

		Projectile->FinishSpawning(SpawnTransform);
	}
}
