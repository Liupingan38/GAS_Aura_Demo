// Copyright Liupingan


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	OnHealthChanged.Broadcast(GetAuraAS()->GetHealth());
	OnMaxHealthChanged.Broadcast(GetAuraAS()->GetMaxHealth());
	OnManaChanged.Broadcast(GetAuraAS()->GetMana());
	OnMaxManaChanged.Broadcast(GetAuraAS()->GetMaxMana());
	
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);
	GetAuraPS()->OnLevelChangedDelegate.AddLambda(
	[this](int32 NewLevel,bool bLevelUp)
		{
			OnPlayerLevelChangedDelegate.Broadcast(NewLevel,bLevelUp);
		}
	);

	
	GetAuraASC()->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);
	GetAuraASC()->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxHealthAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);
	GetAuraASC()->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnManaChanged.Broadcast(Data.NewValue);
		}
	);
	GetAuraASC()->GetGameplayAttributeValueChangeDelegate(GetAuraAS()->GetMaxManaAttribute()).AddLambda(
		[this](const FOnAttributeChangeData& Data)
		{
			OnMaxManaChanged.Broadcast(Data.NewValue);
		}
	);

	if (GetAuraASC())
	{
		GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&UOverlayWidgetController::OnAbilityEquipped);
		
		if (GetAuraASC()->bStartupAbilitiesHasGiven)
		{
			BroadcastAbilityInfo();
		}
		else
		{
			GetAuraASC()->AbilityGivenDelegate.AddUObject(this, &UOverlayWidgetController::BroadcastAbilityInfo);
		}

		//仅分割
		GetAuraASC()->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& TagContainer)
			{
				for (const FGameplayTag& Tag : TagContainer)
				{
					FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
					if (Tag.MatchesTag(MessageTag))
					{
						const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
						MessageWidgetRowDelegate.Broadcast(*Row);
					}
				}
			}
		);
	}
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) 
{
	const ULevelUpInfo* LevelUpInfo = GetAuraPS()->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unable to find LevelUpInfo.Please fill out AuraPlayerState Blueprint"))
	const int32 CurLevel = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num() ;
	if (CurLevel > 0 && CurLevel <= MaxLevel)
	{
		const int32 CurRequirement=LevelUpInfo->LevelUpInformation[CurLevel].LevelUpRequirement;
		const int32 PreRequirement=LevelUpInfo->LevelUpInformation[CurLevel-1].LevelUpRequirement;
		const int32 CurLevelRequirement=CurRequirement - PreRequirement;
		const int32 CurLevelXP=NewXP-PreRequirement;

		const float XPPercent=static_cast<float>(CurLevelXP)/static_cast<float>(CurLevelRequirement);
		OnXPPercentChangedDelegate.Broadcast(XPPercent);
	}
}

void UOverlayWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
	const FGameplayTag& NewSlotTag, const FGameplayTag& PreSlotTag)
{
	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	
	FAuraAbilityInfo PreSlotInfo;
	PreSlotInfo.InputTag=PreSlotTag;
	PreSlotInfo.AbilityTag=Tags.Abilities_None;
	PreSlotInfo.StatusTag=Tags.Abilities_Status_UnEquipped;
	AbilityInfoDelegate.Broadcast(PreSlotInfo);

	FAuraAbilityInfo CurSlotInfo=AbilityInfoDataAsset->FindAbilityInfoForTag(AbilityTag);
	CurSlotInfo.InputTag=NewSlotTag;
	CurSlotInfo.AbilityTag=AbilityTag;
	CurSlotInfo.StatusTag=StatusTag;
	AbilityInfoDelegate.Broadcast(CurSlotInfo);
	
}

