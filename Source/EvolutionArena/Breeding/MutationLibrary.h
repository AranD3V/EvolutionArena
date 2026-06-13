// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Math/RandomStream.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MutationLibrary.generated.h"

struct FGenome;
class UDataTable;

/**
 * The Mutation System: rolls each mutation definition against a seeded RNG stream
 * and records the hits on the genome. Deterministic — the caller owns the seed, and
 * rows are rolled in a stable order, so the same stream state yields the same
 * mutations on every platform. Integer-only per-mille rolls. See docs/rules.md.
 */
UCLASS()
class UMutationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Rolls every mutation row (in stable, lexically-sorted order) and appends each
	 * hit's id to Genome.Mutations. Uses the provided stream so the caller controls
	 * the seed and the position in the RNG sequence.
	 */
	static void RollMutations(FGenome& Genome, FRandomStream& Stream, const UDataTable* MutationTable);
};
