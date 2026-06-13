// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "GeneticsRows.generated.h"

/**
 * How a gene's two alleles combine into the single value that is expressed.
 * Stored per-gene (in Gene_Def) so inheritance behavior is tunable in data.
 */
UENUM(BlueprintType)
enum class EGeneDominance : uint8
{
	/** Express the higher allele — Max(A, B). */
	Dominant,
	/** Express the lower allele — Min(A, B). */
	Recessive,
	/** Blend both — (A + B) / 2, integer division. */
	Codominant
};

/**
 * Static definition of a body part — one row per part in the BodyPart DataTable.
 * The row's FName key *is* the part id (what FPartAssignment.PartId references), so
 * there is no separate id field. StatModifiers are added straight to derived stats.
 * See docs/schema.md §3 (BodyPart_Def).
 */
USTRUCT(BlueprintType)
struct FBodyPartDef : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	EBodyPartSlot Slot = EBodyPartSlot::Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	FStatBlock StatModifiers;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Part")
	ERarity Rarity = ERarity::Common;
};

/**
 * Static definition of a gene — one row per gene in the Gene DataTable. The row's
 * FName key is the gene id (what FGene.GeneDefId references). The expressed allele
 * value (chosen per Dominance) scales StatMapping to produce the gene's stat
 * contribution. Only Type == Stat genes affect stats. See docs/schema.md §3.
 */
USTRUCT(BlueprintType)
struct FGeneDef : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gene")
	EGeneType Type = EGeneType::Stat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gene")
	EGeneDominance Dominance = EGeneDominance::Dominant;

	/** Per-unit stat weight; multiplied by the expressed allele value. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gene")
	FStatBlock StatMapping;
};

/**
 * Static definition of a mutation — one row per mutation in the Mutation DataTable.
 * The row's FName key is the mutation id recorded on a genome when the mutation hits.
 * `ChancePermille` is an integer 0–1000 (per-mille) so the roll is integer-only and
 * deterministic across platforms (no float compares). See docs/schema.md §3.
 */
USTRUCT(BlueprintType)
struct FMutationDef : public FTableRowBase
{
	GENERATED_BODY()

	/** Probability the mutation occurs, in per-mille (0–1000). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation", meta = (ClampMin = "0", ClampMax = "1000"))
	int32 ChancePermille = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
	ERarity Rarity = ERarity::Rare;

	/** Reserved: stat effect for stat-affecting mutations (not applied yet). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
	FStatBlock Effect;

	/** Reserved: which gene category the mutation targets (future use). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mutation")
	EGeneType AffectedType = EGeneType::Special;
};
