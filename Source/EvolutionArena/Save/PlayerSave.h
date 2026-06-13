// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Creature/Creature.h"
#include "PlayerSave.generated.h"

/**
 * Root persisted player data — the player's entire game state. Serialized through
 * the Save/Load subsystem only (never ad hoc). `SaveVersion` drives migration on
 * load. Every field is flagged `SaveGame` so it travels through UE's tagged
 * SaveGame archive (which tolerates added/removed fields across versions).
 * See docs/schema.md §3 (FPlayerSave) and docs/rules.md (Save through the subsystem).
 */
USTRUCT(BlueprintType)
struct FPlayerSave
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	FString PlayerId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	FString DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	int32 Level = 1;

	/** Total accumulated XP; Level is derived from this (see UProgressionLibrary). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	int32 Xp = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	int32 SoftCurrency = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	int32 PremiumCurrency = 0;

	// Ranked stats (additive — old saves default these via the tagged SaveGame archive).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save|Ranked")
	int32 RankedRating = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save|Ranked")
	int32 Wins = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save|Ranked")
	int32 Losses = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	TArray<FCreature> Creatures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Save")
	TArray<FName> EvolutionUnlocks;

	/** On-disk format version; bumped on breaking changes (see USaveSubsystem). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Save")
	int32 SaveVersion = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Save")
	FDateTime LastSavedUtc = FDateTime(0);
};
