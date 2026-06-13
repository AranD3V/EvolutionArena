// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Genetics/EvoGeneticsTypes.h"
#include "Genome.generated.h"

/**
 * One gene, stored as an allele pair. Which allele is *expressed* is decided by
 * the Genetics System using the dominance rule in the gene's DataTable row;
 * the genome only stores the raw pair. GeneDefId references a Gene_Def row.
 * See docs/schema.md §3 (FGene).
 */
USTRUCT(BlueprintType)
struct FGene
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	FName GeneDefId = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	uint8 AlleleA = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	uint8 AlleleB = 0;

	bool operator==(const FGene& Other) const
	{
		return GeneDefId == Other.GeneDefId
			&& AlleleA == Other.AlleleA
			&& AlleleB == Other.AlleleB;
	}
};

/** Assigns one body part (a BodyPart_Def row) to one slot on a creature. */
USTRUCT(BlueprintType)
struct FPartAssignment
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	EBodyPartSlot Slot = EBodyPartSlot::Head;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	FName PartId = NAME_None;

	bool operator==(const FPartAssignment& Other) const
	{
		return Slot == Other.Slot && PartId == Other.PartId;
	}
};

/**
 * The complete genetic definition of a creature: its genes (allele pairs), its
 * body-part assignments, any active mutations, and the cached stat block those
 * produce. DerivedStats is recomputed by the Genetics System; it is read-only
 * to designers/Blueprints because nothing should set it by hand.
 * See docs/schema.md §3 (FGenome).
 */
USTRUCT(BlueprintType)
struct FGenome
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	TArray<FGene> Genes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	TArray<FPartAssignment> Parts;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, SaveGame, Category = "Genome")
	TArray<FName> Mutations;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, Category = "Genome")
	FStatBlock DerivedStats;
};
