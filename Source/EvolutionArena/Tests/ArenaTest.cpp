// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Online/RankingLibrary.h"
#include "Online/LeaderboardService.h"
#include "Online/LocalLeaderboardService.h"
#include "Online/ArenaLibrary.h"
#include "Battle/BattleTypes.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	FCreature MakeFighter(const int32 Health, const int32 Attack, const int32 Defense, const int32 Speed, const FString& Name = TEXT("Fighter"))
	{
		FCreature Creature;
		Creature.CreatureId = FGuid::NewGuid();
		Creature.Name = Name;
		Creature.Genome.DerivedStats.Health = Health;
		Creature.Genome.DerivedStats.Attack = Attack;
		Creature.Genome.DerivedStats.Defense = Defense;
		Creature.Genome.DerivedStats.Speed = Speed;
		return Creature;
	}

	FOpponentSnapshot MakeSnapshot(const FString& Id, const int32 Rating, const FCreature& Loadout)
	{
		FOpponentSnapshot Snapshot;
		Snapshot.PlayerId = Id;
		Snapshot.Rating = Rating;
		Snapshot.Loadout = Loadout;
		return Snapshot;
	}
}

// Rating math: sign, zero-sum-ish behavior, and "beating stronger gains more".
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArenaRatingDeltaTest,
	"EvolutionArena.Arena.RatingDelta",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FArenaRatingDeltaTest::RunTest(const FString&)
{
	TestTrue(TEXT("Win vs equal is positive"),
		URankingLibrary::ComputeRatingDelta(1000, 1000, EBattleOutcome::CombatantAWon) > 0);
	TestTrue(TEXT("Loss vs equal is negative"),
		URankingLibrary::ComputeRatingDelta(1000, 1000, EBattleOutcome::CombatantBWon) < 0);
	TestEqual(TEXT("Draw vs equal is zero"),
		URankingLibrary::ComputeRatingDelta(1000, 1000, EBattleOutcome::Draw), 0);

	const int32 BeatStronger = URankingLibrary::ComputeRatingDelta(1000, 1300, EBattleOutcome::CombatantAWon);
	const int32 BeatWeaker = URankingLibrary::ComputeRatingDelta(1000, 700, EBattleOutcome::CombatantAWon);
	TestTrue(TEXT("Beating a stronger opponent gains more"), BeatStronger > BeatWeaker);
	return true;
}

// Opponent selection picks the closest rating; empty pool returns false.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArenaOpponentSelectionTest,
	"EvolutionArena.Arena.OpponentSelection",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FArenaOpponentSelectionTest::RunTest(const FString&)
{
	ULocalLeaderboardService* Service = NewObject<ULocalLeaderboardService>();
	Service->AddSnapshot(MakeSnapshot(TEXT("low"), 800, MakeFighter(50, 5, 2, 5)));
	Service->AddSnapshot(MakeSnapshot(TEXT("mid"), 1020, MakeFighter(80, 15, 8, 10)));
	Service->AddSnapshot(MakeSnapshot(TEXT("high"), 1500, MakeFighter(120, 30, 10, 20)));

	FOpponentSnapshot Selected;
	TestTrue(TEXT("Found an opponent"), Service->SelectOpponent(1000, Selected));
	TestEqual(TEXT("Closest rating chosen"), Selected.PlayerId, FString(TEXT("mid")));

	ULocalLeaderboardService* Empty = NewObject<ULocalLeaderboardService>();
	FOpponentSnapshot None;
	TestFalse(TEXT("Empty pool -> no opponent"), Empty->SelectOpponent(1000, None));
	return true;
}

// Full ranked match: win raises rating, loss lowers it, and it's deterministic.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FArenaRankedMatchTest,
	"EvolutionArena.Arena.RankedMatch",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FArenaRankedMatchTest::RunTest(const FString&)
{
	ULocalLeaderboardService* Service = NewObject<ULocalLeaderboardService>();
	Service->AddSnapshot(MakeSnapshot(TEXT("weakling"), 1000, MakeFighter(40, 4, 2, 4)));

	const FCreature Strong = MakeFighter(120, 30, 10, 20, TEXT("Champion"));
	FRankedMatchResult Result;
	TestTrue(TEXT("Match ran"), URankedArenaLibrary::RunRankedMatch(Strong, 1000, TEXT("me"), Service, 99, Result));
	TestTrue(TEXT("Player won"), Result.bWin);
	TestTrue(TEXT("Rating went up"), Result.RatingAfter > Result.RatingBefore);
	TestEqual(TEXT("Delta is consistent"), Result.RatingAfter - Result.RatingBefore, Result.RatingDelta);
	TestEqual(TEXT("Opponent recorded"), Result.OpponentId, FString(TEXT("weakling")));

	// Same inputs + seed -> identical battle.
	FRankedMatchResult Again;
	URankedArenaLibrary::RunRankedMatch(Strong, 1000, TEXT("me"), Service, 99, Again);
	TestTrue(TEXT("Deterministic battle timeline"), Result.Battle.Timeline == Again.Battle.Timeline);

	// A weak player loses rating against a strong opponent.
	ULocalLeaderboardService* Service2 = NewObject<ULocalLeaderboardService>();
	Service2->AddSnapshot(MakeSnapshot(TEXT("boss"), 1000, MakeFighter(120, 30, 10, 20)));
	const FCreature Weak = MakeFighter(40, 4, 2, 4, TEXT("Weakling"));
	FRankedMatchResult LossResult;
	URankedArenaLibrary::RunRankedMatch(Weak, 1000, TEXT("me"), Service2, 99, LossResult);
	TestFalse(TEXT("Weak player lost"), LossResult.bWin);
	TestTrue(TEXT("Rating went down"), LossResult.RatingAfter < LossResult.RatingBefore);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
