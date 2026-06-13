// Copyright Evolution Arena. All Rights Reserved.

#include "Misc/AutomationTest.h"
#include "Battle/BattleSimulator.h"
#include "Battle/BattleAI.h"
#include "Battle/BattleTypes.h"
#include "Creature/Creature.h"

#if WITH_DEV_AUTOMATION_TESTS

namespace
{
	// Build a creature with derived stats set directly (bypasses genetics so tests
	// control the exact combat scenario).
	FCreature MakeFighter(const int32 Health, const int32 Attack, const int32 Defense, const int32 Speed)
	{
		FCreature Creature;
		Creature.CreatureId = FGuid::NewGuid();
		Creature.Genome.DerivedStats.Health = Health;
		Creature.Genome.DerivedStats.Attack = Attack;
		Creature.Genome.DerivedStats.Defense = Defense;
		Creature.Genome.DerivedStats.Speed = Speed;
		return Creature;
	}

	FBattleCombatant MakeCombatant(const int32 Attack, const int32 Defense, const int32 CurrentHealth)
	{
		FBattleCombatant Combatant;
		Combatant.Stats.Attack = Attack;
		Combatant.Stats.Defense = Defense;
		Combatant.CurrentHealth = CurrentHealth;
		return Combatant;
	}
}

// THE RELEASE GATE: same creatures + same seed -> byte-identical result and timeline.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBattleDeterminismTest,
	"EvolutionArena.Battle.Determinism",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBattleDeterminismTest::RunTest(const FString&)
{
	const FCreature A = MakeFighter(100, 25, 10, 15);
	const FCreature B = MakeFighter(90, 22, 12, 18);

	const FBattleResult First = UBattleSimulator::SimulateBattle(A, B, 4242);
	const FBattleResult Second = UBattleSimulator::SimulateBattle(A, B, 4242);

	TestTrue(TEXT("Outcome identical"), First.Outcome == Second.Outcome);
	TestEqual(TEXT("Rounds identical"), First.Rounds, Second.Rounds);
	TestEqual(TEXT("Final health A identical"), First.FinalHealthA, Second.FinalHealthA);
	TestEqual(TEXT("Final health B identical"), First.FinalHealthB, Second.FinalHealthB);
	TestTrue(TEXT("Timeline non-empty"), First.Timeline.Num() > 0);
	TestTrue(TEXT("Timeline identical"), First.Timeline == Second.Timeline);
	return true;
}

// A strictly stronger creature beats a weaker one, regardless of seed.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBattleStatsDecideTest,
	"EvolutionArena.Battle.StatsDecide",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBattleStatsDecideTest::RunTest(const FString&)
{
	const FCreature Strong = MakeFighter(120, 30, 10, 20);
	const FCreature Weak = MakeFighter(50, 5, 2, 5);

	for (const int32 Seed : { 1, 2, 3, 7, 99 })
	{
		const FBattleResult Result = UBattleSimulator::SimulateBattle(Strong, Weak, Seed);
		TestTrue(FString::Printf(TEXT("Strong wins (seed %d)"), Seed),
			Result.Outcome == EBattleOutcome::CombatantAWon);
	}
	return true;
}

// Two tanky low-damage creatures still terminate at the round cap (no infinite loop).
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBattleTerminationTest,
	"EvolutionArena.Battle.Termination",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBattleTerminationTest::RunTest(const FString&)
{
	const FCreature Tank1 = MakeFighter(500, 1, 100, 10);
	const FCreature Tank2 = MakeFighter(500, 1, 100, 10);

	const FBattleResult Result = UBattleSimulator::SimulateBattle(Tank1, Tank2, 1);

	TestEqual(TEXT("Hit the round cap"), Result.Rounds, UBattleSimulator::MaxRounds);
	TestTrue(TEXT("Mirror match is a draw"), Result.Outcome == EBattleOutcome::Draw);
	return true;
}

// The AI goes for the kill (Heavy) only when a Heavy would be lethal.
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FBattleAIGoesForKillTest,
	"EvolutionArena.Battle.AIGoesForKill",
	EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FBattleAIGoesForKillTest::RunTest(const FString&)
{
	const FBattleCombatant Self = MakeCombatant(/*Attack*/ 20, /*Defense*/ 0, /*Health*/ 50);
	// HeavyDamage = max(1, (20*3)/2 - 0) = 30.
	const FBattleCombatant LowOpponent = MakeCombatant(0, 0, /*Health*/ 25);  // 25 <= 30 -> Heavy
	const FBattleCombatant HighOpponent = MakeCombatant(0, 0, /*Health*/ 100); // 100 > 30 -> Attack

	TestTrue(TEXT("Heavy when lethal"), UBattleAI::ChooseAction(Self, LowOpponent) == EBattleAction::Heavy);
	TestTrue(TEXT("Attack otherwise"), UBattleAI::ChooseAction(Self, HighOpponent) == EBattleAction::Attack);
	return true;
}

#endif // WITH_DEV_AUTOMATION_TESTS
