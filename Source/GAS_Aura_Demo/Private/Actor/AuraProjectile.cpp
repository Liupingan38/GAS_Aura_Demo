// Copyright Liupingan


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GAS_Aura_Demo/GAS_Aura_Demo.h"
#include "Kismet/GameplayStatics.h"


AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	RootComponent = Sphere;
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	ProjectileMovementComponent->InitialSpeed = 500.f;
	ProjectileMovementComponent->MaxSpeed = 500.f;
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(LiveSpan);
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	LoopingSoundComponent =UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());
	
}

void AAuraProjectile::Destroyed()
{
	if (!HasAuthority() && !bHit)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
		LoopingSoundComponent->Stop();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent,
                                      AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp,
                                      int32 OtherBodyIndex,
                                      bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	LoopingSoundComponent->Stop();
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC=UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
