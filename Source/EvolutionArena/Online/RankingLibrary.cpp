// Copyright Evolution Arena. All Rights Reserved.

#include "Online/RankingLibrary.h"

int32 URankingLibrary::ComputeRatingDelta(int32 SelfRating, int32 OpponentRating, EBattleOutcome Outcome)
{
	// Player is combatant A. Score in per-mille.
	int32 ScorePermille = 500; // Draw
	if (Outcome == EBattleOutcome::CombatantAWon)
	{
		ScorePermille = 1000;
	}
	else if (Outcome == EBattleOutcome::CombatantBWon)
	{
		ScorePermille = 0;
	}

	// Expected score (per-mille): lower when the opponent is higher-rated.
	const int32 ExpectedPermille = FMath::Clamp(500 - (OpponentRating - SelfRating) / RatingScale, 0, 1000);

	// Integer Elo-like update.
	return (RatingKFactor * (ScorePermille - ExpectedPermille)) / 1000;
}
