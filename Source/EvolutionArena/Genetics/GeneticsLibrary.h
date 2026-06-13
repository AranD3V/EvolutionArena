// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "Genetics/GeneticsRows.h"
#include "Genetics/Genome.h"
#include "GeneticsLibrary.generated.h"

/**
 * The Genetics System: the single authority that turns a genome (genes + parts)
 * plus the static content tables into a creature's derived stats.
 *
 * Pure and deterministic — integer-only math, no RNG, no wall-clock — so the same
 * genome always yields the same stats on every platform. This is the rule the
 * battle sim and breeding will rely on. See docs/TechSpec.md §4 and docs/rules.md.
 */
UCLASS()
class UGeneticsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Resolves the expressed allele value of a gene under a dominance rule. */
	UFUNCTION(BlueprintPure, Category = "Genetics")
	static uint8 ResolveExpressedAllele(const FGene& Gene, EGeneDominance Dominance);

	/**
	 * Derived stats = sum of part StatModifiers + sum of expressed Stat-gene
	 * contributions. Base is zero; unknown/missing rows are skipped (use
	 * ValidateGenome to detect them). Deterministic for a given genome + tables.
	 */
	UFUNCTION(BlueprintCallable, Category = "Genetics")
	static FStatBlock ComputeDerivedStats(const FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable);

	/** Convenience: compute and store the result on the genome in place. */
	UFUNCTION(BlueprintCallable, Category = "Genetics")
	static void RecomputeDerivedStats(UPARAM(ref) FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable);

	/**
	 * Verifies every part/gene the genome references exists in the tables (and that
	 * a part's assigned slot matches its definition). Returns true if clean, else
	 * fills OutErrors. Table-wide startup validation can build on this per-genome check.
	 */
	static bool ValidateGenome(const FGenome& Genome, const UDataTable* PartTable, const UDataTable* GeneTable, TArray<FString>& OutErrors);
};
