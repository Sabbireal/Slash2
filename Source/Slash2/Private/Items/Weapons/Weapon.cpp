// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponBox->SetGenerateOverlapEvents(true);

	StarTrace = CreateDefaultSubobject<USceneComponent>(TEXT("Start Trace"));
	StarTrace->SetupAttachment(GetRootComponent());

	EndTrace = CreateDefaultSubobject<USceneComponent>(TEXT("End Trace"));
	EndTrace->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponBoxOverlap);
}

void AWeapon::OnWeaponBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FHitResult HitResult;
	BoxTrace(HitResult);
	ExecuteGetHit(HitResult);
	CreateFields(HitResult.ImpactPoint);
}

void AWeapon::BoxTrace(FHitResult& HitResult)
{
	FVector StarLocation = StarTrace->GetComponentLocation();
	FVector EndLocation = EndTrace->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());

	for (AActor* Actor : IgnoreActor) {
		ActorsToIgnore.AddUnique(Actor);
	}

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		StarLocation,
		EndLocation,
		FVector(2.5f, 2.5f, 2.5f),
		StarTrace->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);
	IgnoreActor.AddUnique(HitResult.GetActor());
}

void AWeapon::ExecuteGetHit(FHitResult& HitResult)
{
	AActor* HittedActor = HitResult.GetActor();
	
	if (!HittedActor || ActorIsSameType(HittedActor)) return;

	if (HittedActor) {
		IHitInterface* HitInterface = Cast<IHitInterface>(HittedActor);

		if (!HitInterface) return;

		UGameplayStatics::ApplyDamage(
			HittedActor,
			DamageAmount,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		HitInterface->Execute_GetHit(HittedActor, HitResult.ImpactPoint, GetOwner());
	}
}

bool AWeapon::ActorIsSameType(AActor* HittedActor)
{
	return GetOwner()->ActorHasTag("Enemy") && HittedActor->ActorHasTag("Enemy");
}


void AWeapon::Equip(USceneComponent* InParent, FName InSocketname, AActor* WeaponOwner, APawn* WeaponInstigator)
{
	SetOwner(WeaponOwner);
	SetInstigator(WeaponInstigator);

	AttachToSocket(InParent, InSocketname);
	ItemState = EItemState::EIS_Equipped;
	
	Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (ItemEffect) {
		ItemEffect->Deactivate();
	}
}

void AWeapon::AttachToSocket(USceneComponent* InParent, const FName& InSocketname)
{
	FAttachmentTransformRules transformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, transformRules, InSocketname);
}
