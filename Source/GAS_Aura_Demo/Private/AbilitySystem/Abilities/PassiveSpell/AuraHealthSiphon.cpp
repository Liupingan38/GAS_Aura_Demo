// Copyright Liupingan


#include "AbilitySystem/Abilities/PassiveSpell/AuraHealthSiphon.h"

FString UAuraHealthSiphon::GetDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT(
		//标题
		"<HealthTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n\n"
		
		//伤害
		//额外链接个数
		"<Default>被动处于激活状态时，每次对敌人造成伤害时有 </><Percent>%d%%</><Default> 的概率将该伤害转化为自身血量。</>"),
						   L"嗜血",
						   AbilityLevel,
						   AbilityLevel*5
	);
}

FString UAuraHealthSiphon::GetNextLevelDescription(int32 NextAbilityLevel)
{
	return FString::Printf(TEXT(
		//标题
		"<HealthTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n\n"
		
		//伤害
		//额外链接个数
		"<Default>被动处于激活状态时，每次对敌人造成伤害时有 </><Percent>%d%%</><Default> 的概率将该伤害转化为自身血量。</>"),
						   L"下一级：",
						   NextAbilityLevel,
						   NextAbilityLevel*5
	);
}
