// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraFireBolt.h"

#include "AuraGameplayTags.h"

FString UAuraFireBolt::GetDescription(int32 AbilityLevel)
{
	const int32 Damage = GetDamageByDamageTypeTag(AbilityLevel,FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost=GetManaCost(AbilityLevel);
	const float Cooldown=GetCooldown(AbilityLevel);
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
		FMath::Min(AbilityLevel,NumProjectiles),
		Damage
	);
}

FString UAuraFireBolt::GetNextLevelDescription(int32 NextAbilityLevel)
{
	const int32 Damage = GetDamageByDamageTypeTag(NextAbilityLevel,FAuraGameplayTags::Get().Damage_Fire);
	const float ManaCost=GetManaCost(NextAbilityLevel);
	const float Cooldown=GetCooldown(NextAbilityLevel);
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
		FMath::Min(NextAbilityLevel,NumProjectiles),
		Damage
	);
}