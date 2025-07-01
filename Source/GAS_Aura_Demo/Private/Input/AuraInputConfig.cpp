// Copyright Liupingan


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                   bool bLogNotFound) const
{
	for (const auto& Info : AbilityInputActions)
	{
		if (Info.InputTag.IsValid() && Info.InputTag.MatchesTagExact(InputTag))
		{
			return Info.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag[%s],on InputConfig[%s]"),
		       *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
