// Copyright Epic Games, Inc. All Rights Reserved.

#include "CrazyCarGameMode.h"
#include "CrazyCarPawn.h"
#include "CrazyCarHud.h"

ACrazyCarGameMode::ACrazyCarGameMode()
{
	DefaultPawnClass = ACrazyCarPawn::StaticClass();
	HUDClass = ACrazyCarHud::StaticClass();
}
