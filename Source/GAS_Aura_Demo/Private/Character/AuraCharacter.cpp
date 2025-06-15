// Copyright Liupingan


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	GetCharacterMovement()->bOrientRotationToMovement=true;
	GetCharacterMovement()->RotationRate=FRotator(0.f,400.f,0.f);
	GetCharacterMovement()->bConstrainToPlane=true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch=false;
	bUseControllerRotationYaw=false;
	bUseControllerRotationRoll=false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor Info for the Server
	InitAbilityActorInfo();
	
	SetOwner(NewController);
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// Init Ability Actor Info for the Client
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState=GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AbilitySystemComponent=AuraPlayerState->GetAbilitySystemComponent();
	check(AbilitySystemComponent);
	AttributeSet=AuraPlayerState->GetAttributeSet();
	check(AttributeSet);
	
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState,this);
	
	if (AAuraPlayerController* AuraPlayerController=Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD=Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController,AuraPlayerState,AbilitySystemComponent,AttributeSet);
		}
	}
}

/*void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	if (!AuraPlayerState) return;

	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	if (!AbilitySystemComponent || !AttributeSet)
	{
		UE_LOG(LogTemp, Warning, TEXT("InitAbilityActorInfo failed: ASC or AttributeSet is null"));
		return;
	}

	// 初始化 GAS 所需信息
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);

	// UI 初始化（仅本地控制客户端需要）
	if (IsLocallyControlled())
	{
		if (AAuraPlayerController* AuraPlayerController = GetController<AAuraPlayerController>())
		{
			if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
			{
				AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("AuraHUD is null"));
			}
		}
	}
}*/


