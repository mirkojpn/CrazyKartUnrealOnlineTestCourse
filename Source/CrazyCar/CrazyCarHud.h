// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/HUD.h"
#include "CrazyCarHud.generated.h"


UCLASS(config = Game)
class ACrazyCarHud : public AHUD
{
	GENERATED_BODY()

public:
	ACrazyCarHud();

	/** Font used to render the vehicle info */
	UPROPERTY()
	UFont* HUDFont;

	// Begin AHUD interface
	virtual void DrawHUD() override;
	// End AHUD interface
};
