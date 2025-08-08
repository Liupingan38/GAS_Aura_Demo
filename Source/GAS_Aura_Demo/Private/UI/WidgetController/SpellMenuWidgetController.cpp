// Copyright Liupingan


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	OnPlayerSpellPointChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoint());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraPS()->OnSpellPointChangedDelegate.AddLambda([this](int32 NewSpellPoint)
	{
		OnPlayerSpellPointChangedDelegate.Broadcast(NewSpellPoint);
		//一旦法术点数改变，重新更新按钮
		CurrentSpellPoint = NewSpellPoint;
		bool bButton_Learn = false;
		bool bButton_Equip = false;
		ShouldEnableButtons(SelectedAbility.AbilityStatusTag, CurrentSpellPoint, bButton_Learn, bButton_Equip);
		FString Description;
		FString NextLevelDescription;
		GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
		OnSpellButtonSelectedDelegate.Broadcast(bButton_Learn, bButton_Equip, Description, NextLevelDescription);
	});

	GetAuraASC()->AbilityStatusChangedDelegate.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
		{
			//一旦法术状态改变，重新更新按钮（选择的法术未改变，但状态改变了）
			if (SelectedAbility.AbilityTag == AbilityTag)
			{
				SelectedAbility.AbilityStatusTag = StatusTag;
				bool bButton_Learn = false;
				bool bButton_Equip = false;
				ShouldEnableButtons(StatusTag, CurrentSpellPoint, bButton_Learn, bButton_Equip);
				FString Description;
				FString NextLevelDescription;
				GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
				OnSpellButtonSelectedDelegate.Broadcast(bButton_Learn, bButton_Equip, Description, NextLevelDescription);
			}
			if (AbilityInfoDataAsset)
			{
				FAuraAbilityInfo Info = AbilityInfoDataAsset->FindAbilityInfoForTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		});

	GetAuraASC()->AbilityEquippedDelegate.AddUObject(this,&USpellMenuWidgetController::OnAbilityEquipped);
}

void USpellMenuWidgetController::SpellButtonSelected(const FGameplayTag& AbilityTag)
{
	if (bWaitForEquipSelection)
	{
		const FGameplayTag SelectedAbilityTypeTag = AbilityInfoDataAsset->FindAbilityInfoForTag(SelectedAbility.AbilityTag).TypeTag;
		StopWaitForEquipSelectionDelegate.Broadcast(SelectedAbilityTypeTag);
		bWaitForEquipSelection = false;
	}

	const FAuraGameplayTags Tags = FAuraGameplayTags::Get();
	FGameplayTag AbilityStatusTag = Tags.Abilities_Status_Locked;
	const bool bTagValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(Tags.Abilities_None);
	const FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetAbilitySpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec != nullptr;
	if (bTagValid && !bTagNone && bSpecValid)
	{
		AbilityStatusTag = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.AbilityTag = AbilityTag;
	SelectedAbility.AbilityStatusTag = AbilityStatusTag;

	const int32 SpellPoint = GetAuraPS()->GetSpellPoint();

	bool bButton_Learn = false;
	bool bButton_Equip = false;
	ShouldEnableButtons(AbilityStatusTag, SpellPoint, bButton_Learn, bButton_Equip);
	FString Description;
	FString NextLevelDescription;
	GetAuraASC()->GetDescriptionByAbilityTag(SelectedAbility.AbilityTag, Description, NextLevelDescription);
	OnSpellButtonSelectedDelegate.Broadcast(bButton_Learn, bButton_Equip, Description, NextLevelDescription);
}

void USpellMenuWidgetController::SpendSpellPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.AbilityTag);
	}
}

void USpellMenuWidgetController::SpellButtonDeselected()
{
	if (bWaitForEquipSelection)
	{
		const FGameplayTag SelectedAbilityTypeTag = AbilityInfoDataAsset->FindAbilityInfoForTag(SelectedAbility.AbilityTag).TypeTag;
		StopWaitForEquipSelectionDelegate.Broadcast(SelectedAbilityTypeTag);
		bWaitForEquipSelection = false;
	}

	SelectedAbility.AbilityTag = FAuraGameplayTags::Get().Abilities_None;
	SelectedAbility.AbilityStatusTag = FAuraGameplayTags::Get().Abilities_Status_Locked;
	OnSpellButtonSelectedDelegate.Broadcast(false, false, UnselectedDescription, UnselectedDescription);
}

void USpellMenuWidgetController::EquipButtonPressed()
{
	const FGameplayTag TypeTag = AbilityInfoDataAsset->FindAbilityInfoForTag(SelectedAbility.AbilityTag).TypeTag;
	WaitForEquipSelectionDelegate.Broadcast(TypeTag);
	bWaitForEquipSelection = true;
}

void USpellMenuWidgetController::EquipSelectionPressed(const FGameplayTag& SlotTag, const FGameplayTag& TypeTag)
{
	if (!bWaitForEquipSelection) return;
	const FGameplayTag SelectedAbilityType=AbilityInfoDataAsset->FindAbilityInfoForTag(SelectedAbility.AbilityTag).TypeTag;
	if (SelectedAbilityType!=TypeTag) return;

	GetAuraASC()->ServerEquipAbility(SelectedAbility.AbilityTag,SlotTag);
}

void USpellMenuWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
                                                   const FGameplayTag& NewSlotTag, const FGameplayTag& PreSlotTag)
{
 	bWaitForEquipSelection = false;
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
	
	StopWaitForEquipSelectionDelegate.Broadcast(AbilityInfoDataAsset->FindAbilityInfoForTag(AbilityTag).TypeTag);
	OnEquipCompletedDelegate.Broadcast(AbilityTag);
	SpellButtonDeselected();
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatusTag, int32 SpellPoint,
                                                     bool& bButton_Learn, bool& bButton_Equip)
{
	bButton_Learn = false;
	bButton_Equip = false;
	if (AbilityStatusTag == FAuraGameplayTags::Get().Abilities_Status_UnLearned)
	{
		bButton_Learn = true;
		bButton_Equip = false;
	}
	else if (AbilityStatusTag == FAuraGameplayTags::Get().Abilities_Status_UnEquipped)
	{
		bButton_Learn = true;
		bButton_Equip = true;
	}
	else if (AbilityStatusTag == FAuraGameplayTags::Get().Abilities_Status_Equipped)
	{
		bButton_Learn = true;
		bButton_Equip = true;
	}
	if (SpellPoint <= 0)
	{
		bButton_Learn = false;
	}
}
