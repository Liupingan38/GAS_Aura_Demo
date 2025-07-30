// Copyright Liupingan


#include "AbilitySystem/Data/AbilityInfo.h"

#include "GAS_Aura_Demo/AuraLogChannels.h"

FAuraAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for (const auto& Info : AbilityInformation)
	{
		if (Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("AbilityInfo[%s]::Info Not Found By AbilityTag[%s]"), *GetNameSafe(this),
		       *AbilityTag.ToString());
	}
	return FAuraAbilityInfo();
}
