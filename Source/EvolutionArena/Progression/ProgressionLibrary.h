// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Save/PlayerSave.h"
#include "ProgressionLibrary.generated.h"

class UDataTable;

/**
 * Progression & economy rewards. Integer and deterministic: a ranked result grants
 * XP + soft currency, level is derived from XP, and level-gated unlocks (data-driven)
 * accumulate on the save. Reward/curve constants are MVP defaults / config candidates.
 */
UCLASS()
class UProgressionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr int32 XpPerLevel = 100;
	static constexpr int32 WinXp = 50;
	static constexpr int32 LossXp = 10;
	static constexpr int32 WinCoins = 25;
	static constexpr int32 LossCoins = 5;

	/** Level for a total XP amount. Level 1 at 0 XP, +1 per XpPerLevel. */
	UFUNCTION(BlueprintPure, Category = "Progression")
	static int32 LevelForXp(int32 Xp);

	/** Apply XP + currency for a ranked result and recompute Level. */
	UFUNCTION(BlueprintCallable, Category = "Progression")
	static void AwardRankedResult(UPARAM(ref) FPlayerSave& Save, bool bWin);

	/** Grant any unlocks whose RequiredLevel <= the player's Level (idempotent). */
	UFUNCTION(BlueprintCallable, Category = "Progression")
	static void ApplyUnlocks(UPARAM(ref) FPlayerSave& Save, const UDataTable* UnlockTable);

	/** True if the unlock id has been granted to the player. */
	UFUNCTION(BlueprintPure, Category = "Progression")
	static bool IsUnlocked(const FPlayerSave& Save, FName UnlockId);
};
