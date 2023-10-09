// Fill out your copyright notice in the Description page of Project Settings.

#include "Hud/HealthbarWidget.h"
#include "Hud/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthbarWidget::SetHealthParcent(float percent)
{
	if (UserHealthBar == nullptr) {
		UserHealthBar = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (UserHealthBar && UserHealthBar->Healthbar) {
		UserHealthBar->Healthbar->SetPercent(percent);
	}
}
