// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "AuraGameplayTags.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FSpellButtonSelectedSignature, bool, bLearnButtonEnabled, bool, bEquipButtonEnabled,
                                              FString, Description, FString, NextLevelDescription);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitForEquipSelectionSignature,const FGameplayTag&,TypeTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquipCompletedSignature,const FGameplayTag&,AbilityTag);

struct FGameplayTag;

struct FSelectedAbility
{
	FGameplayTag AbilityTag = FGameplayTag();
	FGameplayTag AbilityStatusTag = FGameplayTag();
};

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GAS_AURA_DEMO_API USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable)
	FOnPlayerStatChangedSignature OnPlayerSpellPointChangedDelegate;

	UPROPERTY(BlueprintAssignable)
	FSpellButtonSelectedSignature OnSpellButtonSelectedDelegate;
	
	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature WaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FWaitForEquipSelectionSignature StopWaitForEquipSelectionDelegate;

	UPROPERTY(BlueprintAssignable)
	FEquipCompletedSignature OnEquipCompletedDelegate;

	UFUNCTION(BlueprintCallable)
	void SpellButtonSelected(const FGameplayTag& AbilityTag);

	UFUNCTION(BlueprintCallable)
	void SpendSpellPointButtonPressed();
	
	UFUNCTION(BlueprintCallable)
	void SpellButtonDeselected();
	
	UFUNCTION(BlueprintCallable)
    void EquipButtonPressed();

	UFUNCTION(BlueprintCallable)
	void EquipSelectionPressed(const FGameplayTag& SlotTag,const FGameplayTag& TypeTag);

	void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,
							const FGameplayTag& NewSlotTag, const FGameplayTag& PreSlotTag);

private:
	static void ShouldEnableButtons(const FGameplayTag& AbilityStatusTag, int32 SpellPoint, bool& bButton_Learn, bool& bButton_Equip);
	FSelectedAbility SelectedAbility = {FAuraGameplayTags::Get().Abilities_None, FAuraGameplayTags::Get().Abilities_Status_Locked};
	int32 CurrentSpellPoint = 0;
	bool bWaitForEquipSelection = false;
	FString UnselectedDescription = TEXT("<Unselected>点击技能拂去黑暗</>");
	FGameplayTag SelectedSlot;
};
