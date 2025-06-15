// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"

#include "AuraWidgetController.generated.h"

class UAttributeSet;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet=nullptr;

	FWidgetControllerParams(){}
	
	FWidgetControllerParams(APlayerController* PC,APlayerState* PS,UAbilitySystemComponent* ASC,UAttributeSet* AS)
		:PlayerController(PC),PlayerState(PS),AbilitySystemComponent(ASC),AttributeSet(AS){}
};


UCLASS()
class GAS_AURA_DEMO_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetPlayerControllerParams(const FWidgetControllerParams& WCParams);

	virtual void BroadcastInitialValues();
	virtual void BindCallbacksToDependencies();
	
protected:
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	
	UPROPERTY(BlueprintReadOnly,Category="WidgetController")
	TObjectPtr<UAttributeSet> AttributeSet;
	
};
