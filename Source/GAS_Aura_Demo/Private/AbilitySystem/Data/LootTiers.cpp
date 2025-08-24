// Copyright Liupingan


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems()
{
	TArray<FLootItem> ReturnItems;
	for (const FLootItem& LootItem : LootItemsList)
	{
		for (int i = 0; i < LootItem.MaxNumberToSpawn; ++i)
		{
			if (FMath::RandRange(1.f, 100.f) < LootItem.ChanceToSpawn)
			{
				FLootItem NewLootItem;
				NewLootItem.ItemClass = LootItem.ItemClass;
				NewLootItem.bLootLevelOverride = LootItem.bLootLevelOverride;
				ReturnItems.Add(NewLootItem);
			}
		}
	}
	return ReturnItems;
}
