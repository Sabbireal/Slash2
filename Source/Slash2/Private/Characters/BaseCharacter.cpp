// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributesComponent.h"
#include "Kismet/GameplayStatics.h"
#include "MotionWarpingComponent.h"
#include "Components/CapsuleComponent.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AttributesComponent = CreateDefaultSubobject<UAttributesComponent>("AttributesComponent");
	MotionWarping = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CombatTarget == nullptr) return;
	MotionWarping->AddOrUpdateWarpTargetFromLocation("TranslationUpdate", GetTranslationWarpTarget());
	MotionWarping->AddOrUpdateWarpTargetFromLocation("RotationUpdate", GetRotationWarpTarget());
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) {
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::AttackEnd()
{
}

bool ABaseCharacter::IsAlive()
{
	return AttributesComponent && AttributesComponent->IsAlive();
}

void ABaseCharacter::SpawnBloodEffect(const FVector& ImpactPoint)
{
	if (BloodEffect && GetWorld()) {
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			BloodEffect,
			ImpactPoint
		);
	}
}

void ABaseCharacter::HandleDamage(const float& DamageAmount)
{
	if (AttributesComponent)
		AttributesComponent->ReceiveDamage(DamageAmount);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	const FVector LowaredImpactPoint(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (LowaredImpactPoint - GetActorLocation()).GetSafeNormal();

	const double CosTheta = FVector::DotProduct(Forward, ToHit);

	double Theta = FMath::Acos(CosTheta);
	Theta = FMath::RadiansToDegrees(Theta);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0) {
		Theta *= -1.f;
	}

	FName SectionName("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		SectionName = FName("FromFront");
	}
	else if (Theta >= -145.f && Theta < -45.f)
	{
		SectionName = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 145.f)
	{
		SectionName = FName("FromRight");
	}

	PlayHitMontage(SectionName);

	/*
	if (GEngine) {

		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta : %f"), Theta));
	}

	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 100.f, 5.f, FLinearColor::Green, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 100.f, 5.f, FLinearColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FLinearColor::Blue, 5.f);
	*/
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));
	PlayDeathMontage();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !Montage) return;

	AnimInstance->Montage_Play(Montage);
	AnimInstance->Montage_JumpToSection(SectionName);
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	int32 selection = FMath::RandRange(0, SectionNames.Num() - 1);
	PlayMontageSection(Montage, SectionNames[selection]);
	return selection;
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackSectionNames);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	int32 Selection = PlayRandomMontageSection(DeathMontage, DeathSectionNames);

	TEnumAsByte<EDeathType> pose(Selection);

	if (pose < EDeathType::EDT_MAX) {
		DeathType = pose;
	}

	return Selection;
}

void ABaseCharacter::PlayDodgeMontage()
{
	PlayMontageSection(DodgeMontage, FName("Dodge"));
}

void ABaseCharacter::DodgeEnd()
{

}

void ABaseCharacter::PlayHitMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (!AnimInstance || !HitReactionMontage) return;

	AnimInstance->Montage_Play(HitReactionMontage);
	AnimInstance->Montage_JumpToSection(SectionName);
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (!EquippedWeapon) return;
	EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
	EquippedWeapon->IgnoreActor.Empty();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive())
	{
		DirectionalHitReact(Hitter->GetActorLocation());
	}
	else Die();
	SpawnBloodEffect(ImpactPoint);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget) {
		FVector TargetLocation = CombatTarget->GetActorLocation();
		FVector toME = (GetActorLocation()- TargetLocation).GetSafeNormal();
		toME *= WarpTargetDistance;

		return TargetLocation + toME;
	}
	return FVector();
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget) {
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance) {
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}


