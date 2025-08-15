// Copyright Liupingan


#include "AbilitySystem/Abilities/PassiveSpell/AuraHaloOfProtection.h"

FString UAuraHaloOfProtection::GetDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT(
		//标题
		"<HaloTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n\n"
		
		//伤害
		//额外链接个数
		"<Default>被动处于激活状态时，每次受到敌人攻击有 </><Percent>%d%%</><Default> 的概率免疫该伤害。</>"),
						   L"护佑",
						   AbilityLevel,
						   AbilityLevel*5
	);
}

FString UAuraHaloOfProtection::GetNextLevelDescription(int32 NextAbilityLevel)
{
	return FString::Printf(TEXT(
		//标题
		"<HaloTitle>%s</>\n\n"
		//等级
		"<Small>当前等级：</><Level>%d</>\n\n"
		
		//伤害
		//额外链接个数
		"<Default>被动处于激活状态时，每次受到敌人攻击有 </><Percent>%d%%</><Default> 的概率免疫该伤害。</>"),
						   L"下一级：",
						   NextAbilityLevel,
						   NextAbilityLevel*5
	);
}
