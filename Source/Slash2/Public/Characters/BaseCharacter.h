// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterTypes.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAnimMontage;
class UAttributesComponent;
class UMotionWarpingComponent;

UCLASS()
class SLASH2_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly)
	AWeapon* EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Montage)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	UAnimMontage* DodgeMontage;

	UPROPERTY(EditDefaultsOnly, Category = VisualEffects)
	UParticleSystem* BloodEffect;

	UPROPERTY(VisibleAnywhere)
	UAttributesComponent* AttributesComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMotionWarpingComponent* MotionWarping;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	virtual bool IsAlive();
	virtual void SpawnBloodEffect(const FVector& ImpactPoint);
	virtual void HandleDamage(const float& DamageAmount);

	virtual void Attack();
	virtual bool CanAttack();
	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual void Die();

	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	virtual void PlayDodgeMontage();
	virtual void DodgeEnd();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EDeathType> DeathType;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TArray<FName> AttackSectionNames;

	UPROPERTY(EditDefaultsOnly, Category = Montage)
	TArray<FName> DeathSectionNames;

	void PlayHitMontage(const FName& SectionName);

	UFUNCTION(BlueprintPure)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintPure)
	FVector GetRotationWarpTarget();

	void StopAttackMontage();

public:
	FORCEINLINE EDeathType GetDeathType() const { return DeathType; }
};
