// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	void InitializeLoadSlot();

	UPROPERTY(BlueprintAssignable)
	FSlotSelected SlotSelected;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	UFUNCTION(BlueprintPure)
	UMVVM_LoadSlot * GetLoadSlotByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 InSlotIndex);
	
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 InSlotIndex,const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 InSlotIndex);

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	UFUNCTION(BlueprintCallable)
	void RUSDeleteButtonPressed();

	void LoadData();

	/** Field Notifies */

	void SetLoadScreenName(FString InLoadScreenName);
	FString GetLoadScreenName() const {return LoadScreenName;}
	

	
private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlotMap;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> SelectedSlot;


	UPROPERTY(EditAnywhere,BlueprintReadWrite,FieldNotify,Setter,Getter,meta=(AllowPrivateAccess="true"));
	FString LoadScreenName;
};
