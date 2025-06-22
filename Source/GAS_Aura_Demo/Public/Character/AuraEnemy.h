// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraEnemy : public AAuraCharacterBase,public IEnemyInterface
{
	GENERATED_BODY()
public:
	AAuraEnemy();

	//~ Begin Enemy Interface
	virtual void Highlight() override;
	virtual void UnHighlight() override;
	//~ End Enemy Interface

	//~ Begin Combat Interface
	virtual int32 GetPlayLevel() override;
	//~ Begin Combat Interface
protected:
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Character Class Defaults")
	int32 Level=1;
};
