// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

class AItem;
class ASoul;
class ATreasure;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class SLASH2_API IPickupInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(AItem* Item);
	virtual void AddSouls(ASoul* Soul);
	virtual void AddGold(ATreasure* Soul);
};
