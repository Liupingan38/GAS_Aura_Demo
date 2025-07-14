// Copyright Liupingan


#include "AbilitySystem/Data/CharacterClassInfo.h"

FCharacterClassDefaultInfo UCharacterClassInfo::GetCharacterClassInfo(ECharacterClass CharacterClass)
{
	return CharacterClassInfoMap.FindChecked(CharacterClass);
}
