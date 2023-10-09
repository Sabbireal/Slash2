// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/SlashCharacter.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/Axis.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"
#include "Components/AttributesComponent.h"
#include "Items/Treasures/Treasure.h"
#include "Items/Soul.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	ViewCamera->SetupAttachment(CameraBoom);

	HairGroom = CreateDefaultSubobject<UGroomComponent>(TEXT("HairGroomComponent"));
	HairGroom->SetupAttachment(GetMesh());
	HairGroom->AttachmentName = FString("head");

	EyeLashGroom = CreateDefaultSubobject<UGroomComponent>(TEXT("EyeLashGroomComponent"));
	EyeLashGroom->SetupAttachment(GetMesh());
	EyeLashGroom->AttachmentName = FString("head");
}


void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("EngageableTarget"));

	InitializeSlashOverlay();
	AddingMappingContext();
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AttributesComponent && SlashOverlay) {
		AttributesComponent->RegenStamina(DeltaTime);
		SlashOverlay->SetStaminaBarPercent(AttributesComponent->GetStaminaPercent());
	}
}

void ASlashCharacter::InitializeSlashOverlay()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;
	ASlashHud* SlashHud = Cast<ASlashHud>(playerController->GetHUD());
	if (!SlashHud) return;
	SlashOverlay = SlashHud->GetSlashOverlay();

	if (!SlashOverlay) return;
	if (!AttributesComponent) return;

	SlashOverlay->SetHealthBarPercent(AttributesComponent->GetHealthPercent());
}

void ASlashCharacter::AddingMappingContext()
{
	APlayerController* playerController = Cast<APlayerController>(GetController());
	if (!playerController) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(playerController->GetLocalPlayer());

	if (!Subsystem) return;

	Subsystem->AddMappingContext(MappingContext, 0);
}

void ASlashCharacter::Move(const FInputActionValue& value)
{
	if (actionState != EActionState::EAS_Unoccupied) return;

	if (!GetController()) return;

	const FVector2D MovementValue = value.Get<FVector2D>();

	FRotator ControllerRotation = GetControlRotation();
	FRotator YawRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);

	FVector Forward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	FVector Right = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, MovementValue.Y);
	AddMovementInput(Right, MovementValue.X);
}

void ASlashCharacter::Look(const FInputActionValue& value)
{
	if (!GetController()) return;

	const FVector2D LookDirection = value.Get<FVector2D>();

	AddControllerYawInput(LookDirection.X);
	AddControllerPitchInput(LookDirection.Y);
}

void ASlashCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	
	if (OverlappingWeapon) {
		EquipWeapon(OverlappingWeapon);
	}
	else if (CanDisarm()) {
		Disarm();
	}
	else if (CanArm()) {
		Arm();
	}
}

void ASlashCharacter::Dodge()
{
	if (actionState != EActionState::EAS_Unoccupied) return;
	if (AttributesComponent == nullptr) return;
	if (AttributesComponent->GetStamina() < AttributesComponent->GetDodgeCost()) return;

	actionState = EActionState::EAS_Dodge;
	PlayDodgeMontage();

	AttributesComponent->UseStamina(AttributesComponent->GetDodgeCost());
	
	if (SlashOverlay) {
		SlashOverlay->SetStaminaBarPercent(AttributesComponent->GetStaminaPercent());
	}
}

void ASlashCharacter::DodgeEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::EquipWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	characterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	EquippedWeapon = OverlappingWeapon;
}

void ASlashCharacter::Jump()
{
	if (actionState == EActionState::EAS_Unoccupied) {
		Super::Jump();
	}
}

void ASlashCharacter::Attack()
{
	Super::Attack();
	if (!CanAttack()) return;

	actionState = EActionState::EAS_Attacking;
	PlayAttackMontage();
}

bool ASlashCharacter::CanAttack()
{
	return actionState == EActionState::EAS_Unoccupied && 
		characterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::HandleDamage(const float& DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (SlashOverlay && AttributesComponent) {
		SlashOverlay->SetHealthBarPercent(AttributesComponent->GetHealthPercent());
	}
}

void ASlashCharacter::Die()
{
	Super::Die();
	actionState = EActionState::EAS_Dead;
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !EquipeMontage) return;

	AnimInstance->Montage_Play(EquipeMontage);
	AnimInstance->Montage_JumpToSection(SectionName);
}

bool ASlashCharacter::CanArm()
{
	return actionState == EActionState::EAS_Unoccupied &&
		characterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

bool ASlashCharacter::CanDisarm()
{
	return actionState == EActionState::EAS_Unoccupied &&
		characterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::Arm()
{
	PlayEquipMontage(FName("Equipe"));
	characterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	actionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequipe"));
	characterState = ECharacterState::ECS_Unequipped;
	actionState = EActionState::EAS_EquippingWeapon;
}

void ASlashCharacter::AttackEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::AttachWeaponToHand()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ASlashCharacter::AttachWeaponToBack()
{
	if (EquippedWeapon) {
		EquippedWeapon->AttachToSocket(GetMesh(), FName("SplineSocket"));
	}
}

void ASlashCharacter::EndEquipping()
{
	actionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInputComponent) return;

	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Look);
	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Jump);
	EnhancedInputComponent->BindAction(EKeyPressedAction, ETriggerEvent::Triggered, this, &ASlashCharacter::EKeyPressed);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Attack);
	EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Dodge);
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (IsAlive()) {
		actionState = EActionState::EAS_HitReaction;
	}
	
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	return DamageAmount;
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (AttributesComponent) {
		AttributesComponent->AddSouls(Soul->GetSouls());
	
		if (SlashOverlay) {
			SlashOverlay->SetSouls(AttributesComponent->GetSouls());
		}
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (AttributesComponent) {
		AttributesComponent->AddGold(Treasure->GetGold());

		if (SlashOverlay) {
			SlashOverlay->SetGold(AttributesComponent->GetGold());
		}
	}
}

void ASlashCharacter::HitReactEnd()
{
	actionState = EActionState::EAS_Unoccupied;
}

