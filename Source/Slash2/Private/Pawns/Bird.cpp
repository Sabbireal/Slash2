// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("BirdCapsuleComponent"));
	Capsule->SetCapsuleHalfHeight(20.f);
	Capsule->SetCapsuleRadius(15.f);

	SetRootComponent(Capsule);

	BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMeshComponent"));
	BirdMesh->SetupAttachment(GetRootComponent());

	PawnMovement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("PawnMovemnt"));
	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	ViewCamera->SetupAttachment(CameraBoom);
	
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* playerContoller = Cast<APlayerController>(GetController());
	
	if (playerContoller) 
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(playerContoller->GetLocalPlayer());
		
		if (SubSystem) {
			SubSystem->AddMappingContext(BirdMappingContext, 0);
		}
	}
}

void ABird::Move(const FInputActionValue& value)
{
	if (!GetController()) return;

	const float movementDirection = value.Get<float>();
	if (movementDirection == 0) return;

	if (GEngine) {
		char pressed = ((movementDirection < 0) ? 'S' : 'W');
		FString message = FString::Printf(TEXT("Pressed %c"), pressed);
		GEngine->AddOnScreenDebugMessage(2, 0.f, FColor::Blue, message);
	}

	AddMovementInput(GetActorForwardVector(), movementDirection);
}

void ABird::Look(const FInputActionValue& value)
{
	if (!GetController()) return;

	const FVector2D lookDirection = value.Get<FVector2D>();
	
	AddControllerYawInput(lookDirection.X);
	AddControllerPitchInput(lookDirection.Y);
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) return;

	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABird::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABird::Look);
}

