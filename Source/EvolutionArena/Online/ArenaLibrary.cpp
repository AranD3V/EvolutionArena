// Copyright Evolution Arena. All Rights Reserved.

#include "Online/ArenaLibrary.h"
#include "Online/LeaderboardService.h"
#include "Online/RankingLibrary.h"
#include "Battle/BattleSimulator.h"

bool URankedArenaLibrary::RunRankedMatch(const FCreature& PlayerLoadout, int32 PlayerRating, const FString& PlayerId,
	ULeaderboardService* Service, int32 Seed, FRankedMatchResult& OutResult)
{
	if (!Service)
	{
		return false;
	}

	FOpponentSnapshot Opponent;
	if (!Service->SelectOpponent(PlayerRating, Opponent))
	{
		return false; // no opponent in the pool
	}

	// Player is always combatant A.
	const FBattleResult Battle = UBattleSimulator::SimulateBattle(PlayerLoadout, Opponent.Loadout, Seed);
	const bool bWin = Battle.Outcome == EBattleOutcome::CombatantAWon;
	const int32 Delta = URankingLibrary::ComputeRatingDelta(PlayerRating, Opponent.Rating, Battle.Outcome);
	const int32 NewRating = PlayerRating + Delta;

	Service->SubmitResult(PlayerId, NewRating, bWin);

	OutResult.Battle = Battle;
	OutResult.RatingBefore = PlayerRating;
	OutResult.RatingAfter = NewRating;
	OutResult.RatingDelta = Delta;
	OutResult.bWin = bWin;
	OutResult.OpponentId = Opponent.PlayerId;
	return true;
}
