// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ProgressionRows.generated.h"

/**
 * Static definition of a progression unlock — one row per unlock. The row's FName
 * key is the unlock id added to FPlayerSave.EvolutionUnlocks once the player reaches
 * RequiredLevel; content gates by checking that set. See docs/schema.md §3.
 */
USTRUCT(BlueprintType)
struct FProgressionUnlockDef : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Progression", meta = (ClampMin = "1"))
	int32 RequiredLevel = 1;
};
