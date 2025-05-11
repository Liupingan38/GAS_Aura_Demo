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

protected:
	virtual void BeginPlay() override;
	
};
