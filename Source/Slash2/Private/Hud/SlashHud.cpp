// Fill out your copyright notice in the Description page of Project Settings.


#include "Hud/SlashHud.h"
#include "Hud/SlashOverlay.h"

void ASlashHud::BeginPlay()
{
	Super::BeginPlay();

	if (SlashOverlayClass && GWorld) {
		APlayerController* playerController = GWorld->GetFirstPlayerController();
		if (playerController) {
			SlashOverlay = CreateWidget<USlashOverlay>(playerController, SlashOverlayClass);
			SlashOverlay->AddToViewport();
		}
	}
}
