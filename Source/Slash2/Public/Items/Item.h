// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;
class UNiagaraSystem;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class SLASH2_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	   
	AItem();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rotation Parameter")
		float RotationSpeed = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin Parameter")
		float Amplitude = 0.1f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sin Parameter")
		float TimeConstant = 5.f;

	

	UFUNCTION(BlueprintPure, Category = "Custom Functions")
		float TransformedSin();

	UFUNCTION(BlueprintPure, Category = "Custom Functions")
		float TransformedCos();

	UFUNCTION(BlueprintCallable, Category = "Custom Functions")
	void RotateYawContinuosly(float deltaTime);

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	template<typename T>
	T AVG(T first, T second);

	UFUNCTION()
	virtual void OnSphereOverlap(
			UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, 
			bool bFromSweep, 
			const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnSphereEndOverlap(
			 UPrimitiveComponent* OverlappedComponent,
			 AActor* OtherActor, 
			 UPrimitiveComponent* OtherComp,
			 int32 OtherBodyIndex
	);

	void SpawnPickupEffect();

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(VisibleAnywhere, Category = Effects)
	UNiagaraComponent* ItemEffect;

private:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Sin Parameter", meta = (AllowPrivateAccess = "true"))
	float RunningTime;

	UPROPERTY(EditAnywhere)
	UNiagaraSystem* PickupEffect;
};

template<typename T>
inline T AItem::AVG(T first, T second)
{
	return (first + second) / 2;
}
