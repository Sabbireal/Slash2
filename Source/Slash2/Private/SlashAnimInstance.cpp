// Fill out your copyright notice in the Description page of Project Settings.


#include "SlashAnimInstance.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void USlashAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	SlashCharacter = Cast<ASlashCharacter>(TryGetPawnOwner());
	if (!SlashCharacter) return;

	MovementComponent = SlashCharacter->GetCharacterMovement();
}

void USlashAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!MovementComponent) return;

	GroundSpeed = UKismetMathLibrary::VSizeXY(MovementComponent->Velocity);
	IsFalling = MovementComponent->IsFalling();
	CharacterState = SlashCharacter->GetCharacterState();
	ActionState = SlashCharacter->GetActionState();
	DeathType = SlashCharacter->GetDeathType();
}
