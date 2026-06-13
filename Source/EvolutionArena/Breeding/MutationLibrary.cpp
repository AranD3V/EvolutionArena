// Copyright Evolution Arena. All Rights Reserved.

#include "Breeding/MutationLibrary.h"
#include "Genetics/Genome.h"
#include "Genetics/GeneticsRows.h"
#include "Engine/DataTable.h"

void UMutationLibrary::RollMutations(FGenome& Genome, FRandomStream& Stream, const UDataTable* MutationTable)
{
	if (!MutationTable)
	{
		return;
	}

	// Roll rows in a stable, lexically-sorted order. RowMap iteration order is not
	// guaranteed, and FName::FastLess depends on registration order (varies per run),
	// so we sort by LexicalLess to keep the RNG draw sequence identical everywhere.
	TArray<FName> RowNames = MutationTable->GetRowNames();
	RowNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });

	static const FString Context(TEXT("RollMutations"));
	for (const FName& RowName : RowNames)
	{
		const FMutationDef* Def = MutationTable->FindRow<FMutationDef>(RowName, Context, /*bWarnIfMissing=*/ false);
		if (!Def)
		{
			continue;
		}

		// Integer roll: [0,999] < per-mille chance. No floats in the RNG path.
		if (Stream.RandRange(0, 999) < Def->ChancePermille)
		{
			Genome.Mutations.AddUnique(RowName);
		}
	}
}
