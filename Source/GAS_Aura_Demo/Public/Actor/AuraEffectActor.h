// Copyright Liupingan

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "GameFramework/Actor.h"
#include "AuraEffectActor.generated.h"


class UAbilitySystemComponent;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EEffectApplicationPolicy:uint8
{
	ApplyOnOverlay,
	ApplyOnEndOverlay,
	DoNotApply
};

UENUM(BlueprintType)
enum class EEffectRemovalPolicy:uint8
{
	RemoveOnEndOverlay,
	DoNotRemove
};


UCLASS()
class GAS_AURA_DEMO_API AAuraEffectActor : public AActor
{
	GENERATED_BODY()

public:
	AAuraEffectActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass);

	UFUNCTION(BlueprintCallable)
	void OnOverlap(AActor* TargetActor);

	UFUNCTION(BlueprintCallable)
	void OnEndOverlap(AActor* TargetActor);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	bool bDestroyOnEffectApplication = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	bool bApplyEffectsToEnemy = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InstantGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy InstantEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> DurationGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy DurationEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	TSubclassOf<UGameplayEffect> InfiniteGameplayEffectClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectApplicationPolicy InfiniteEffectApplicationPolicy = EEffectApplicationPolicy::DoNotApply;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Applied Effects")
	EEffectRemovalPolicy InfiniteEffectRemovalPolicy = EEffectRemovalPolicy::RemoveOnEndOverlay;

	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, UAbilitySystemComponent*> ActiveEffectHandles;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Applied Effects")
	float ActorLevel = 1.f;

	UPROPERTY(BlueprintReadWrite)
	FVector CalculateLocation;
	UPROPERTY(BlueprintReadWrite)
	FRotator CalculateRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	bool bRotates = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float RotationRate = 45.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	bool bSineMovement = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SineAmplitude = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	float SinePeriodConstant = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup Movement")
	FVector InitialLocation;
	
	UFUNCTION(BlueprintCallable)
	void StartSineMovement();
	UFUNCTION(BlueprintCallable)
	void StartRotation();
	
private:
	float RunningTime = 0.f;

	void ItemMovement(float DeltaTime);
};
