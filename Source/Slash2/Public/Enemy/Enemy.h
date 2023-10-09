// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthbarWidget;
class AAIController;
class UPawnSensingComponent;
class AWeapon;
class ASoul;

UCLASS()
class SLASH2_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	
	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage
	(
		float DamageAmount,
		struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator,
		AActor* DamageCauser
	) override;

	virtual void Destroyed() override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;

protected:
	virtual void BeginPlay() override;
	virtual void AttackEnd() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyStates EnemyState = EEnemyStates::EES_Patrolling;

	UPROPERTY()
	AAIController* AIController;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* CurrentPatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float  WaitMin = 50.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float  WaitMax = 10.f;


	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void HandleDamage(const float& DamageAmount) override;
	virtual void Die() override;
	void SpawnSoul();

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.2f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;
private:
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hud", meta = (AllowPrivateAccess = true))
	UHealthbarWidget* HealthbarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSense;

	void SetHealthbarVisibility(bool IsVisible);

	UPROPERTY(EditAnywhere)
	bool ShowDebugPath = false;
	void DrawDebugPath(FNavPathSharedPtr& NavPath);

	void MoveToTarget(AActor* Target);
	bool IsInRange(AActor* Target, float Radious);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	double  CombatRadious = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	double  AttackRadious = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	double  AttackStopDistance = 60.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UFUNCTION()
	void PawnSeen(APawn* Pawn);

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASoul> SoulClass;

	/// refactored code///
	void SpawnWeapon();
	void LoseInterest();

	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	bool IsDead();

	bool IsOutsideCombatRadious();
	bool IsOutsideAttackRadious();
	bool IsInsideAttackRadious();

	void StartPatrolling();
	void StartChasing();

	void StartAttackTimer();
	void StartPatrolTimer();
	void ClearAttackTimer();
	void ClearPatrolTimer();
	AActor* ChoosePatrolTarget();
};
