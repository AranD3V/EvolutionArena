// Copyright Evolution Arena. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Creature/Creature.h"
#include "LeaderboardService.generated.h"

/** A stored opponent the player battles asynchronously (a "ghost"). */
USTRUCT(BlueprintType)
struct FOpponentSnapshot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	FString PlayerId;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	int32 Rating = 1000;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	FCreature Loadout;
};

/** One leaderboard standing. */
USTRUCT(BlueprintType)
struct FRankEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	FString PlayerId;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	int32 Rating = 1000;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	int32 Wins = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Arena")
	int32 Losses = 0;
};

/**
 * Abstract online leaderboard / matchmaking service. ALL ranked online access goes
 * through this interface (docs/rules.md: "online behind interfaces"). A local
 * in-memory implementation backs dev/offline/tests; an EOS-backed implementation is
 * the only thing that changes to go live — arena/ranking code never calls EOS directly.
 */
UCLASS(Abstract)
class ULeaderboardService : public UObject
{
	GENERATED_BODY()

public:
	/** Record a completed ranked match for a player. */
	virtual void SubmitResult(const FString& PlayerId, int32 NewRating, bool bWin)
		PURE_VIRTUAL(ULeaderboardService::SubmitResult, );

	/** Pick an opponent snapshot near the given rating. False if none available. */
	virtual bool SelectOpponent(int32 PlayerRating, FOpponentSnapshot& OutOpponent) const
		PURE_VIRTUAL(ULeaderboardService::SelectOpponent, return false;);

	/** Top N standings, highest rating first. */
	virtual TArray<FRankEntry> GetTopEntries(int32 Count) const
		PURE_VIRTUAL(ULeaderboardService::GetTopEntries, return TArray<FRankEntry>(););
};
