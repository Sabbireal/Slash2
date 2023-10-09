// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributesComponent.h"

UAttributesComponent::UAttributesComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributesComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributesComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributesComponent::ReceiveDamage(float damage)
{
	Health = FMath::Clamp(Health - damage, 0, MaxHealth);
}

float UAttributesComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

bool UAttributesComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributesComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0, MaxStamina);
}

float UAttributesComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

void UAttributesComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + (StaminaRegenRate * DeltaTime), 0, MaxStamina);
}

void UAttributesComponent::AddGold(int32 AmountOfGold)
{
	Gold += AmountOfGold;
}

void UAttributesComponent::AddSouls(int32 NumberOfSouls)
{
	Souls += NumberOfSouls;
}



