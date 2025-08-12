// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraBeamSpell.h"

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraBeamSpell::StoreMouseHitDataInfo(const FHitResult& HitResult)
{
	if (HitResult.bBlockingHit)
	{
		MouseHitActor = HitResult.GetActor();
		MouseHitLocation = HitResult.ImpactPoint;
	}
	else
	{
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void UAuraBeamSpell::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void UAuraBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> OverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);

	UAuraAbilitySystemLibrary::GetLivePlayerWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors,
	                                                     ActorsToIgnore, 500.f, MouseHitActor->GetActorLocation());
	int32 NumAdditionalTargets = FMath::Min(MaxNumAdditionalTargets, GetAbilityLevel());

	UAuraAbilitySystemLibrary::GetClosestTargets(MouseHitActor->GetActorLocation(), MaxNumAdditionalTargets, OverlappingActors,
	                                             OutAdditionalTargets);

	for (AActor* AdditionalActor:OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface=Cast<ICombatInterface>(AdditionalActor))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::AdditionalTargetDied);
			}
		}
	}
}

void UAuraBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter);
	if (OwnerCharacter->Implements<UCombatInterface>())
	{
		if (const USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));

			FHitResult HitResult;
			TArray<AActor*> ActorsToIgnore;
			ActorsToIgnore.Add(OwnerCharacter);
			UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f,
			                                        TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::None, HitResult, true);

			if (HitResult.bBlockingHit)
			{
				MouseHitActor = HitResult.GetActor();
				MouseHitLocation = HitResult.ImpactPoint;
			}
		}
	}
	if (ICombatInterface* CombatInterface=Cast<ICombatInterface>(MouseHitActor))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this,&UAuraBeamSpell::FirstTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this,&UAuraBeamSpell::FirstTargetDied);
		}
	}
}
