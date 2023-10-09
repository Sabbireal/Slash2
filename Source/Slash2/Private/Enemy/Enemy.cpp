// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributesComponent.h"
#include "Hud/HealthbarWidget.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthbarWidget = CreateDefaultSubobject<UHealthbarWidget>(TEXT("HealthbarWidget"));
	HealthbarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	PawnSense = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSense"));
	PawnSense->SightRadius = 1500.0f;
	PawnSense->SetPeripheralVisionAngle(45.f);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	Tags.Add(FName("Enemy"));

	AIController = Cast<AAIController>(GetController());
	
	if (PawnSense) {
		PawnSense->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	InitializeEnemy();
}

void AEnemy::InitializeEnemy()
{
	CurrentPatrolTarget = ChoosePatrolTarget();
	StartPatrolling();
	SetHealthbarVisibility(false);
	SpawnWeapon();
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyStates::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::SpawnWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass) {
		AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = Weapon;
	}
}

void AEnemy::LoseInterest()
{
	SetHealthbarVisibility(false);
	CombatTarget = nullptr;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyStates::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyStates::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyStates::EES_Engaged;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyStates::EES_Dead;
}

bool AEnemy::IsOutsideCombatRadious()
{
	return !IsInRange(CombatTarget, CombatRadious);
}

bool AEnemy::IsOutsideAttackRadious()
{
	return !IsInRange(CombatTarget, AttackRadious);
}

bool AEnemy::IsInsideAttackRadious()
{
	return IsInRange(CombatTarget, AttackRadious);
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyStates::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(CurrentPatrolTarget);
}

void AEnemy::StartChasing()
{
	EnemyState = EEnemyStates::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;
	if (EnemyState > EEnemyStates::EES_Patrolling) {
		CheckCombatTarget();
	}
	else {
		CheckPatrolTarget();
	}
}

void AEnemy::CheckCombatTarget()
{
	//if (CombatTarget == nullptr) return;

	if (IsOutsideCombatRadious()) {
		ClearAttackTimer();
		LoseInterest();
		if(!IsEngaged())StartPatrolling();
	}
	else if (IsOutsideAttackRadious() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged())StartChasing();
	}
	else if (CanAttack()) {
		StartAttackTimer();
	}
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadious() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();
}

void AEnemy::Attack()
{
	Super::Attack();
	if (CombatTarget == nullptr) return;
	
	EnemyState = EEnemyStates::EES_Engaged;
	PlayAttackMontage();
}

void AEnemy::HandleDamage(const float& DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (HealthbarWidget) {
		HealthbarWidget->SetHealthParcent(AttributesComponent->GetHealthPercent());
	}
}
void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyStates::EES_Attacking;
	float AttackDelay = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackDelay);
}

void AEnemy::StartPatrolTimer()
{
	CurrentPatrolTarget = ChoosePatrolTarget();
	const float WaitingDuration = FMath::RandRange(WaitMin, WaitMax);
	GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitingDuration);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}


void AEnemy::CheckPatrolTarget()
{
	if (IsInRange(CurrentPatrolTarget, PatrolRange)) {

		StartPatrolTimer();
	}
}
AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Actor : PatrolTargets)
	{
		if (Actor == CurrentPatrolTarget) continue;
		ValidTargets.AddUnique(Actor);
	}

	const int32 ValidTargetsCount = ValidTargets.Num();
	if (ValidTargetsCount == 0) return nullptr;

	const int32 selection = FMath::RandRange(0, ValidTargetsCount - 1);
	return ValidTargets[selection];
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (AIController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(AttackStopDistance);

	FNavPathSharedPtr NavPath;

	//AIController->MoveToActor(Target, AttackStopDistance);
	
	AIController->MoveTo(MoveRequest, &NavPath);
	DrawDebugPath(NavPath);
}

bool AEnemy::IsInRange(AActor* Target, float Radious)
{
	if (Target == nullptr) return false;
	
	const float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radious;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(CurrentPatrolTarget);
}

void AEnemy::PawnSeen(APawn* Pawn)
{
	if (EnemyState != EEnemyStates::EES_Patrolling || 
		!Pawn->ActorHasTag(FName("EngageableTarget"))
		) return;

	CombatTarget = Pawn;
	ClearPatrolTimer();
	StartChasing();
}

void AEnemy::DrawDebugPath(FNavPathSharedPtr& NavPath)
{
	if (!ShowDebugPath) return;
	if (NavPath == nullptr) return;

	TArray<FNavPathPoint> Points = NavPath->GetPathPoints();

	UWorld* World = GetWorld();

	if (World == nullptr) return;

	for (FNavPathPoint Point : Points) {
		const FVector PointLocation = Point.Location;
		DrawDebugSphere(World, PointLocation, 20.f, 10, FColor::Green, false, 20.f);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (IsAlive())
	{
		SetHealthbarVisibility(true);
	}

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	StopAttackMontage();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();

	StartChasing();
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (EquippedWeapon) {
		EquippedWeapon->Destroy();
	}
}

void AEnemy::Die()
{
	Super::Die();

	EnemyState = EEnemyStates::EES_Dead;
	SetHealthbarVisibility(false);
	SetLifeSpan(5.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	SpawnSoul();
}

void AEnemy::SpawnSoul()
{
	UWorld* World = GetWorld();

	if (World && SoulClass && AttributesComponent) 
	{
		ASoul* SpawnedSoul = World->SpawnActor <ASoul> (SoulClass, GetActorLocation(), GetActorRotation());
		
		if (SpawnedSoul) {
			SpawnedSoul->SetSouls(AttributesComponent->GetSouls());
		}
	}
}

void AEnemy::SetHealthbarVisibility(bool IsVisible)
{
	if (HealthbarWidget && HealthbarWidget->IsVisible() != IsVisible) {
		HealthbarWidget->SetVisibility(IsVisible);
	}
}

