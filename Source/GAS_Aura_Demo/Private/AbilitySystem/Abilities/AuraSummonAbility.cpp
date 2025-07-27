// Copyright Liupingan


#include "AbilitySystem/Abilities/AuraSummonAbility.h"


TArray<FVector> UAuraSummonAbility::GetSpawnLocation()
{
	TArray<FVector> SpawnLocationArray;
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector();

	const FVector LeftDirection = Forward.RotateAngleAxis(-SpawnSpreadAngle / 2, FVector::UpVector);
	const float DeltaAngle = SpawnSpreadAngle / (NumMinion - 1);

	for (int32 i = 0; i < NumMinion; i++)
	{
		FVector CurDirection = LeftDirection.RotateAngleAxis(DeltaAngle * i, FVector::UpVector);
		FVector SpawnLocation = Location + CurDirection * FMath::RandRange(MinSpawnDistance, MaxSpawnDistance);

		//生成再地面上，上下坡也可
		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, SpawnLocation + FVector(0.f, 0.f, 400.f),
		                                     SpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if (Hit.bBlockingHit)
		{
			SpawnLocation = Hit.ImpactPoint;
		}

		SpawnLocationArray.Add(SpawnLocation);
	}

	return SpawnLocationArray;
}

TSubclassOf<APawn> UAuraSummonAbility::GetRandomMinionClass()
{
	const int32 Selection=FMath::RandRange(0,MinionClassArray.Num()-1);
	return MinionClassArray[Selection];
}
