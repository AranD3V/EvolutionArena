// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Online/LeaderboardService.h"
#include "LocalLeaderboardService.generated.h"

/**
 * In-memory leaderboard service for development, offline play, and tests. Selection
 * and standings are fully deterministic. Swapped for an EOS-backed service later.
 */
UCLASS()
class ULocalLeaderboardService : public ULeaderboardService
{
	GENERATED_BODY()

public:
	/** Seed the opponent pool (also registers a baseline rank entry). */
	void AddSnapshot(const FOpponentSnapshot& Snapshot);

	virtual void SubmitResult(const FString& PlayerId, int32 NewRating, bool bWin) override;
	virtual bool SelectOpponent(int32 PlayerRating, FOpponentSnapshot& OutOpponent) const override;
	virtual TArray<FRankEntry> GetTopEntries(int32 Count) const override;

private:
	UPROPERTY()
	TArray<FOpponentSnapshot> Snapshots;

	UPROPERTY()
	TMap<FString, FRankEntry> Entries;
};
