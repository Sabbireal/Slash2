// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class UBoxComponent;

UCLASS()
class SLASH2_API AWeapon : public AItem
{
	GENERATED_BODY()

protected:

	UFUNCTION()
	void OnWeaponBoxOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* StarTrace;

	UPROPERTY(VisibleAnywhere)
	USceneComponent* EndTrace;
	
	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float DamageAmount = 20.f;

public:
	AWeapon();
	virtual void BeginPlay() override;
	
	void Equip(USceneComponent* InParent, FName InSocketname, AActor* WeaponOwner, APawn* WeaponInstigator);
	void AttachToSocket(USceneComponent* InParent, const FName& InSocketname);
	
	TArray<AActor*> IgnoreActor;

private:
	void ExecuteGetHit(FHitResult& HitResult);
	void BoxTrace(FHitResult& HitResult);
	bool ActorIsSameType(AActor* HittedActor);

public:
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }
};
