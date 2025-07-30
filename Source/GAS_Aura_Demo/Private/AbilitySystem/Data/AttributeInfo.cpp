// Copyright Liupingan


#include "AbilitySystem/Data/AttributeInfo.h"

#include "GAS_Aura_Demo/AuraLogChannels.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bNotFound) const
{
	for (const auto& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}
	if (bNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Info for AttributeTag [%s] on AttributeInfo [%s] not found."), *AttributeTag.ToString(),
		       *GetNameSafe(this));
	}
	return FAuraAttributeInfo();
}
