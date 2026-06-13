// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "Genetics/Genome.h"
#include "Creature/Creature.h"
#include "BreedingLibrary.generated.h"

class UDataTable;

/**
 * The Breeding System: combines two parent genomes into an offspring (Mendelian
 * per-locus inheritance), rolls mutations, recomputes derived stats, and assigns
 * rarity. The *genome* is fully deterministic for a given pair of parents + seed
 * (the creature's id/timestamp are not — each offspring is a unique entity).
 * See docs/rules.md (Determinism) and docs/TechSpec.md §7.
 */
UCLASS()
class UBreedingLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Produce an offspring genome from two parents using the seeded stream. */
	static FGenome BreedGenomes(const FGenome& ParentA, const FGenome& ParentB, FRandomStream& Stream,
		const UDataTable* GeneTable, const UDataTable* PartTable, const UDataTable* MutationTable);

	/** MVP rarity: Σ part rarities + 2×mutation count, mapped to a tier. */
	UFUNCTION(BlueprintPure, Category = "Breeding")
	static ERarity CalculateRarity(const FGenome& Genome, const UDataTable* PartTable);

	/**
	 * Full breeding operation: a deterministic offspring genome (for the given seed)
	 * wrapped in a new creature with fresh id, lineage, rarity, and timestamp. Does
	 * NOT check cooldown or deduct cost — that is the breeding transaction, handled
	 * separately (see docs/implementationplan.md).
	 */
	UFUNCTION(BlueprintCallable, Category = "Breeding")
	static FCreature BreedCreatures(const FCreature& ParentA, const FCreature& ParentB, int32 Seed,
		const UDataTable* GeneTable, const UDataTable* PartTable, const UDataTable* MutationTable);
};
