// Fill out your copyright notice in the Description page of Project Settings.


#include "Hud/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar) {
		HealthProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar) {
		StaminaProgressBar->SetPercent(Percent);
	}
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldText) {
		const FString GoldString = FString::Printf(TEXT("%d"), Gold);
		const FText GText = FText::FromString(GoldString);
		GoldText->SetText(GText);
	}
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if (SoulText) {
		const FString SoulsString = FString::Printf(TEXT("%d"), Souls);
		const FText SText = FText::FromString(SoulsString);
		SoulText->SetText(SText);
	}
}
