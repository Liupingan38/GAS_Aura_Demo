// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AuraCharacterBase.generated.h"

UCLASS(Abstract)
class GAS_AURA_DEMO_API AAuraCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	
	AAuraCharacterBase();

protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;
};
