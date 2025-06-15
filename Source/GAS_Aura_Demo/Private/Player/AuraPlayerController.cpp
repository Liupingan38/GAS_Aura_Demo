// Copyright Liupingan


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates=true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility,false,CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor=ThisActor;
	ThisActor=CursorHit.GetActor();
	if (LastActor==nullptr)
	{
		if (ThisActor!=nullptr) ThisActor->Highlight();
	}
	else  //LastActor!=nullptr
	{
		if (ThisActor==nullptr)
		{
			LastActor->UnHighlight();
		}
		else if (LastActor!=ThisActor)
		{
			LastActor->UnHighlight();
			ThisActor->Highlight();
		}
	}
	
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext); //判断是否存在

	//从本地角色身上获取到它的子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0); //可以存在多个操作映射，根据优先级触发
	}
	
	bShowMouseCursor = true; //游戏中是否显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default; //鼠标光标的样式

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //将鼠标锁定在视口内
	InputModeData.SetHideCursorDuringCapture(false); //鼠标被捕获时是否隐藏
	SetInputMode(InputModeData); //设置给控制器
	
}

void AAuraPlayerController::SetupInputComponent()
{  
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent=CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction,ETriggerEvent::Triggered,this,&AAuraPlayerController::Move);
	
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector2D=InputActionValue.Get<FVector2D>(); 
	const FRotator Rotator=GetControlRotation();
	const FRotator YawRotator=FRotator(0.f,Rotator.Yaw,0.f);
	const FVector ForwardDirection=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Type::X);
	const FVector RightDirection=FRotationMatrix(YawRotator).GetUnitAxis(EAxis::Type::Y);

	if (APawn* ControlledPawn=GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection,InputAxisVector2D.Y);
		ControlledPawn->AddMovementInput(RightDirection,InputAxisVector2D.X);
	}
}


