// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s</><Level>%d</>"), L"这其实是一段默认文本，技能等级：", AbilityLevel);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 NextAbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s</><Level>%d</> \n<Default> next level</>"), L"这其实是一段下一等级默认文本，当前技能等级：", NextAbilityLevel);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 AbilityLevel)
{
	return FString::Printf(TEXT("<Default>%s</><Level>%d</><Default>%s</>"), L"该技能等级 ", AbilityLevel, L" 解锁。");
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	//获取到冷却GE
	if(const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		//遍历GE修改的内容
		for(FGameplayModifierInfo Mod : CostEffect->Modifiers)
		{
			//判断修改的属性是否为角色蓝量属性
			if(Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				//通过修饰符获取到使用的FScalableFloat并计算传入等级的蓝量消耗，FScalableFloat是受保护性的属性，无法直接获取，只能通过函数
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break; //获取到了就结束遍历
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	//获取到技能冷却GE
	if(const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		//获取到当前冷却时间
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
	
}
