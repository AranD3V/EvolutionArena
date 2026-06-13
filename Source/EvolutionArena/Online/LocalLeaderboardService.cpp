// Copyright Evolution Arena. All Rights Reserved.

#include "Online/LocalLeaderboardService.h"

void ULocalLeaderboardService::AddSnapshot(const FOpponentSnapshot& Snapshot)
{
	Snapshots.Add(Snapshot);

	FRankEntry& Entry = Entries.FindOrAdd(Snapshot.PlayerId);
	Entry.PlayerId = Snapshot.PlayerId;
	Entry.Rating = Snapshot.Rating;
}

void ULocalLeaderboardService::SubmitResult(const FString& PlayerId, int32 NewRating, bool bWin)
{
	FRankEntry& Entry = Entries.FindOrAdd(PlayerId);
	Entry.PlayerId = PlayerId;
	Entry.Rating = NewRating;
	if (bWin)
	{
		++Entry.Wins;
	}
	else
	{
		++Entry.Losses;
	}
}

bool ULocalLeaderboardService::SelectOpponent(int32 PlayerRating, FOpponentSnapshot& OutOpponent) const
{
	const FOpponentSnapshot* Best = nullptr;
	int32 BestDelta = MAX_int32;

	for (const FOpponentSnapshot& Snapshot : Snapshots)
	{
		const int32 Delta = FMath::Abs(Snapshot.Rating - PlayerRating);
		// Closest rating wins; deterministic tie-break by lower PlayerId.
		if (Delta < BestDelta || (Best && Delta == BestDelta && Snapshot.PlayerId < Best->PlayerId))
		{
			Best = &Snapshot;
			BestDelta = Delta;
		}
	}

	if (!Best)
	{
		return false;
	}
	OutOpponent = *Best;
	return true;
}

TArray<FRankEntry> ULocalLeaderboardService::GetTopEntries(int32 Count) const
{
	TArray<FRankEntry> All;
	Entries.GenerateValueArray(All);

	All.Sort([](const FRankEntry& A, const FRankEntry& B)
	{
		if (A.Rating != B.Rating)
		{
			return A.Rating > B.Rating;
		}
		return A.PlayerId < B.PlayerId; // stable tie-break
	});

	if (Count >= 0 && Count < All.Num())
	{
		All.SetNum(Count);
	}
	return All;
}
