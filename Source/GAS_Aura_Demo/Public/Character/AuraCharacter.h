// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UNiagaraComponent;
/**
 * 
 */
UCLASS()
class GAS_AURA_DEMO_API AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Stunned() override;
	virtual void OnRep_Burned() override;
	
	//~ Begin Player Interface
	virtual void AddToXP_Implementation(int32 InXP) override;
	virtual void AddToLevel_Implementation(int32 InLevel) override;
	virtual void AddToAttributePoint_Implementation(int32 InAttributePoint) override;
	virtual void AddToSpellPoint_Implementation(int32 InSpellPoint) override;
	virtual void LevelUp_Implementation() override;
	virtual int32 GetXP_Implementation() const override;
	virtual int32 FindLevelForXP_Implementation(int32 InXP) const override;
	virtual int32 GetAttributePointReward_Implementation(int32 InLevel) const override;
	virtual int32 GetSpellPointReward_Implementation(int32 InLevel) const override;
	virtual int32 GetAttributePoint_Implementation() const override;
	virtual int32 GetSpellPoint_Implementation() const override;
	virtual void SaveProgress_Implementation(const FName& CheckPointTag) override;
	//~ End Player Interface

	//~ Begin Combat Interface
	virtual int32 GetCombatCharacterLevel_Implementation() override;
	virtual void Die(const FVector& DeathImpulse) override;
	//~ End Combat Interface

	UPROPERTY(EditDefaultsOnly)
	float DeathTime=5.f;

	FTimerHandle DeathTimerHandle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;

	void LoadProgress();
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraBoom;
	
	virtual void InitAbilityActorInfo() override;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastLevelUpParticles() const;
	
	
};
