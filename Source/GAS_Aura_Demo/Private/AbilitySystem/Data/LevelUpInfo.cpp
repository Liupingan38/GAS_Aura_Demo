// Copyright Liupingan


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	if (LevelUpInformation.Num()<3) return Level;
	
	bool bSearching = true;
	while (bSearching)
	{
		if (XP>=LevelUpInformation[Level].LevelUpRequirement)
		{
			Level++;
		}else
		{
			bSearching = false;
		}
	}
	return Level;
}
