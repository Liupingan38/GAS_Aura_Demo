// Copyright Liupingan


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	const float SinePeriod = 2.f * PI / SinePeriodConstant;
	if (RunningTime > SinePeriod)
	{
		RunningTime = 0.f;
	}
	ItemMovement(DeltaTime);
}

void AAuraEffectActor::ItemMovement(float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		CalculateRotation = UKismetMathLibrary::ComposeRotators(DeltaRotation, CalculateRotation);
	}
	if (bSineMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		CalculateLocation=InitialLocation+FVector(0.f,0.f,Sine);
	}
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	InitialLocation = GetActorLocation();
	CalculateLocation = InitialLocation;
	CalculateRotation = GetActorRotation();
}

void AAuraEffectActor::StartSineMovement()
{
	bSineMovement = true;
	InitialLocation = GetActorLocation();
	CalculateLocation = InitialLocation;
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculateRotation = GetActorRotation();
}



void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	//目标为敌人 且 不想敌人受到效果物影响时 ，直接返回
	if ((TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemy)) return;

	UAbilitySystemComponent* TargetACS = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetACS == nullptr) return;
	check(GameplayEffectClass);

	FGameplayEffectContextHandle EffectContextHandle = TargetACS->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	FGameplayEffectSpecHandle EffectSpecHandle = TargetACS->MakeOutgoingSpec(
		GameplayEffectClass, ActorLevel, EffectContextHandle);

	FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetACS->ApplyGameplayEffectSpecToSelf(
		*EffectSpecHandle.Data.Get());

	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlay)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetACS);
	}
	if (!bIsInfinite && bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	//目标为敌人 且 不想敌人受到效果物影响时 ，直接返回
	if ((TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemy)) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlay)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlay)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlay)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	//目标为敌人 且 不想敌人受到效果物影响时 ，直接返回
	if ((TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemy)) return;

	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlay)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlay)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlay)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlay)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto HandlePair : ActiveEffectHandles)
		{
			if (HandlePair.Value == TargetASC)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
