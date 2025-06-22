// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()
public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	//~ Begin Combat Interface
	virtual int32 GetPlayLevel() override;
	//~ Begin Combat Interface
private:
	virtual void InitAbilityActorInfo() override;
};
