// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "HealthbarWidget.generated.h"

class UHealthBar;

UCLASS()
class SLASH2_API UHealthbarWidget : public UWidgetComponent
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	void SetHealthParcent(float percent);

private:

	UHealthBar* UserHealthBar;
};
