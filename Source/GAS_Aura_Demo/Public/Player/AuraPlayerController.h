// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

struct FGameplayTag;
struct FInputActionValue;
class IHighlightInterface;
class UNiagaraSystem;
class UDamageTextComponent;
class USplineComponent;
class UAuraAbilitySystemComponent;
class UInputAction;
class UInputMappingContext;
class UAuraInputConfig;

enum class ETargetingStatus:uint8
{
	Enemy,
	Ally,
	Neutral,
	None
};

UCLASS()
class GAS_AURA_DEMO_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount,ACharacter* TargetCharacter, bool bIsBlockedHit, bool bIsCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed() { bShiftPressed = true; }
	void ShiftReleased() { bShiftPressed = false; }
	bool bShiftPressed = false;

	void Move(const FInputActionValue& InputActionValue);

	static void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);
	void CursorTrace();
	FHitResult CursorHit;
	UPROPERTY()
	TObjectPtr<AActor> LastActor;
	UPROPERTY()
	TObjectPtr<AActor> ThisActor;
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();

	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.f;
	float ShortPressedThreshold = 0.5f;
	bool bAutoRun = false;
	ETargetingStatus TargetingStatus = ETargetingStatus::None;

	UPROPERTY(EditAnywhere)
	float AutoRunAcceptanceRadius = 20.f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	
	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
