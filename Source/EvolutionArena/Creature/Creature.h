// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "Genetics/Genome.h"
#include "Creature.generated.h"

/**
 * A single owned creature — the unit stored in the player's collection.
 * Holds a stable id, display name, its genome, derived rarity/evolution state,
 * lineage (parent ids; invalid for built or starter creatures), and the breeding
 * cooldown timestamp. Lineage/id/timestamps are VisibleAnywhere (read-only in
 * editor) because gameplay sets them, not designers. See docs/schema.md §3.
 */
USTRUCT(BlueprintType)
struct FCreature
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Creature")
	FGuid CreatureId;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Creature")
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Creature")
	FGenome Genome;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Creature")
	ERarity Rarity = ERarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Creature")
	int32 EvolutionStage = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Creature|Lineage")
	FGuid ParentA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Creature|Lineage")
	FGuid ParentB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Creature")
	FDateTime BreedCooldownUntil = FDateTime(0);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Creature")
	FDateTime CreatedUtc = FDateTime(0);

	/** True if this creature can breed at the given UTC time. */
	bool IsOffCooldown(const FDateTime NowUtc) const
	{
		return NowUtc >= BreedCooldownUntil;
	}
};
