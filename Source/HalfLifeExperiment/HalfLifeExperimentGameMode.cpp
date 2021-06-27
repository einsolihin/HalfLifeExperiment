// Copyright Epic Games, Inc. All Rights Reserved.

#include "HalfLifeExperimentGameMode.h"
#include "HalfLifeExperimentHUD.h"
#include "HalfLifeExperimentCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHalfLifeExperimentGameMode::AHalfLifeExperimentGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AHalfLifeExperimentHUD::StaticClass();
}
