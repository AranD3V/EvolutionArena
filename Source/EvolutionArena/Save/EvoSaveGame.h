// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Save/PlayerSave.h"
#include "EvoSaveGame.generated.h"

/**
 * USaveGame container for player data. Using a USaveGame means persistence goes
 * through UE's tagged SaveGame archive (only `SaveGame`-flagged properties, and
 * resilient to layout changes between versions) rather than raw binary.
 */
UCLASS()
class UEvoSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	FPlayerSave Data;
};
