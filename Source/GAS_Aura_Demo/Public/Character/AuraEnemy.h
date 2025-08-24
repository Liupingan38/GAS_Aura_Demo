// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "Interaction/HighlightInterface.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

struct FCharacterClassDefaultInfo;
class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface, public IHighlightInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();
	virtual void PossessedBy(AController* NewController) override;

	//~ Begin Highlight Interface
	virtual void Highlight_Implementation() override;
	virtual void UnHighlight_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	//~ Begin Highlight Interface

	//~ Begin Enemy Interface
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	//~ End Enemy Interface

	//~ Begin Combat Interface
	virtual int32 GetCombatCharacterLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	//~ Begin Combat Interface

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	UPROPERTY(BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;

	void SetLevel(const int32 NewLevel) { Level = NewLevel; }

protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat|Character Class Defaults")
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UFUNCTION(BlueprintImplementableEvent)
	void SpawnLoot();
};
