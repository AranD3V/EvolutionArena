# Implementation Plan

> **Purpose:** Defines *in what order* the product gets built — phases,
> milestones, tasks, and dependencies.
> **Read it** when planning work or picking the next task.
> **Update it** when the plan, milestones, or sequencing change.
> Day-to-day status lives in `tracker.md`; this file is the roadmap.

- **Last updated:** 2026-06-13
- **Related:** `PRD.md` (what), `TechSpec.md` (how), `tracker.md` (where we are)

---

## 1. Approach

Build a **thin end-to-end vertical slice first**, then add breadth. The data
layer (creature/genome structs + DataTables for parts/genes) comes before the
systems that consume it, because everything — creation, breeding, mutation,
battle, save — reads the same genome. Within the slice the order is: create a
creature → fight a deterministic battle → see a result → save it. Once that loop
runs, layer on breeding/mutation, then ranked + cloud + progression. Online (EOS)
is wired early but kept behind interfaces so the game runs offline during dev.

Guiding principles: ship the loop before the breadth; keep all balance in
DataTables; protect battle determinism with tests from day one.

## 2. Milestones

| Milestone | Goal / outcome | Target |
|---|---|---|
| M0 — Setup | UE5 project, Android **and Windows** build pipelines, EOS wired, CI, runnable skeleton | 2026-06-27 |
| M1 — Creature Core | Data-driven creature creation + genetics with live stat preview; local save | 2026-07-25 |
| M2 — Breeding & Mutation | Breeding produces offspring; mutations roll; cooldowns; reveal flow | 2026-08-22 |
| M3 — Battle & AI | Deterministic auto-battle + creature AI; battle playback | 2026-09-19 |
| M4 — Arena & Progression (MVP) | Ranked vs snapshots, EOS leaderboards, cloud save, progression, onboarding | 2026-10-17 |
| Alpha | Balance pass, content depth, polish, perf on target devices | 2026-12-15 |

## 3. Phases & tasks

> Small, ordered, verifiable tasks. Statuses live in `tracker.md`, not here.

### Phase 0 — Setup (M0)

- [x] **Project skeleton** — UE5 project, module layout, source control.
  - *Depends on:* nothing
  - *Acceptance:* Project opens, builds, runs to an empty hub.
  - *Done 2026-06-13:* `.uproject` + `Source/` primary module + `Config/` + `.gitignore`;
    editor target compiles clean against UE 5.6; VS solution generates; `git init` done.
    Editor GUI now opens clean (2026-06-14) via the explicit `D:\UE_5.6` path — empty
    default level, no project `Content/` yet (first runnable map is the next M0 step).
- [ ] **Windows build** — Win64 (x86-64, D3D12) packaging; runs to the empty hub.
  - *Depends on:* Project skeleton
  - *Acceptance:* Windows build launches and reaches the hub (fastest test surface
    while a mid-range Android device is sourced).
- [ ] **Android pipeline** — arm64/Vulkan packaging, deploy to a test device.
  - *Depends on:* Project skeleton
  - *Acceptance:* APK installs and launches on a mid-range Android device.
- [ ] **EOS integration shell** — Online Subsystem EOS, login flow behind an interface.
  - *Depends on:* Project skeleton
  - *Acceptance:* Player can authenticate; identity available to game code.
- [ ] **CI + determinism test harness** — automated build + a battle-repro test stub.
  - *Depends on:* Project skeleton
  - *Acceptance:* CI builds and runs automation tests green; the determinism repro
    test runs on **both Win64 and Android** with identical results (cross-architecture
    parity gate).

### Phase 1 — Creature Core (M1)

- [x] **Core data structs** — `FGenome`, `FGene`, `FCreature`, `FStatBlock`.
  - *Depends on:* Phase 0
  - *Acceptance:* Structs compile; serialize/deserialize round-trips.
  - *Done 2026-06-13:* reflected `USTRUCT`s in `Source/EvolutionArena/{Genetics,Creature}/`
    + enums; editor target compiles; `GenomeSerializationRoundTrip` automation test passes.
- [~] **DataTables for parts & genes** — `BodyPart_Def`, `Gene_Def` with a starter set.
  - *Acceptance:* Tables load; rows validated at startup.
  - *Done 2026-06-13:* row structs `FBodyPartDef`/`FGeneDef` (`Genetics/GeneticsRows.h`),
    seed JSON in `Data/`, and `ValidateGenome` (tested). **Pending:** import the JSON
    into `DT_BodyParts`/`DT_Genes` `.uasset`s in-editor + wire startup validation.
- [x] **Genetics System** — allele dominance → expressed genes → derived stats.
  - *Depends on:* Core data structs, DataTables
  - *Acceptance:* Given a genome, stats are deterministic and match expected.
  - *Done 2026-06-13:* `UGeneticsLibrary` (compute/recompute/dominance/validate);
    4 automation tests pass (StatComputation, DominanceResolution, Determinism,
    ContentValidation). Integer-only, data-driven. (See TechSpec §7.)
- [ ] **Creature Assembly + Lab UI** — pick parts per slot, live stat preview, confirm.
  - *Depends on:* Genetics System
  - *Acceptance:* Player builds a creature; stats update live; creature persists.
- [x] **Local Save subsystem** — versioned `FPlayerSave`, atomic write.
  - *Acceptance:* Collection survives app restart; `SaveVersion` migration path exists.
  - *Done 2026-06-14:* `USaveSubsystem` + `UEvoSaveGame` (`Save/`); atomic temp→rename
    write, tagged SaveGame archive, `MigrateIfNeeded` hook. 3 tests pass (RoundTrip,
    AtomicWrite, Migration). Built before the Lab UI: UMG is editor-bound, Save is
    pure C++ depending only on the data structs. (See TechSpec §7.)

### Phase 2 — Breeding & Mutation (M2)

- [x] **Breeding System** — combine two genomes (per-gene allele selection).
  - *Depends on:* Genetics System
  - *Acceptance:* Offspring genome is valid and inherits parent alleles correctly.
  - *Done 2026-06-14:* `UBreedingLibrary::BreedGenomes`/`BreedCreatures` (`Breeding/`);
    Mendelian per-locus inheritance, seeded `FRandomStream`, stable draw order.
    Tests: Inheritance, Determinism. (See TechSpec §7.)
- [x] **Mutation System** — roll chances from `Mutation_Def`, inject rare genes.
  - *Depends on:* Breeding System
  - *Acceptance:* Mutation rate matches config over N samples (statistical test).
  - *Done 2026-06-14:* `UMutationLibrary::RollMutations` + `FMutationDef` (integer
    per-mille) + seed JSON. `MutationRate` test (25% over 4000 trials). Records hit
    ids on the genome; stat-affecting mutations deferred.
- [x] **Rarity calculation** — derive rarity from parts/genes/mutations.
  - *Acceptance:* Deterministic rarity for a given genome (formula per open question).
  - *Done 2026-06-14:* `UBreedingLibrary::CalculateRarity` = `Σ part rarities +
    2×mutations`, thresholds → tiers (MVP default for the PRD open question).
- [x] **Cooldowns + cost** — breeding gates on cooldown/currency.
  - *Acceptance:* Off-cooldown check enforced; cost deducted.
  - *Done 2026-06-14:* `UBreedingLibrary::CanBreed`/`CommitBreed` — distinct parents,
    off-cooldown, affordable; deducts `BreedCost=50`, stamps `BreedCooldown=4h` on both
    parents, adds offspring to the save. 2 tests pass. (Resolves the PRD cooldown question.)
- [ ] **Breeding & reveal UI** — select parents, animate offspring + mutation reveal.
  - *Depends on:* Breeding, Mutation, Save
  - *Acceptance:* Full breed flow works end to end and persists offspring.

### Phase 3 — Battle & AI (M3)

- [x] **Deterministic battle sim** — fixed-step, seed-driven, no rendering dep.
  - *Depends on:* Genetics System
  - *Acceptance:* Same loadouts + seed → identical result (automation test).
  - *Done 2026-06-14:* `UBattleSimulator::SimulateBattle` (`Battle/`); turn-based by
    Speed, integer damage, seeded `FRandomStream`, `MaxRounds` cap, records an
    `FBattleResult` timeline. **Determinism gate test passes.** (See TechSpec §7.)
- [x] **Creature AI** — utility/behavior decisions queried by the sim.
  - *Depends on:* Battle sim
  - *Acceptance:* Outcomes vary sensibly with stats/traits; still deterministic.
  - *Done 2026-06-14:* `UBattleAI::ChooseAction` — go-for-the-kill utility (Heavy when
    lethal, else Attack); deterministic (no RNG in the choice). Tests: StatsDecide,
    AIGoesForKill.
- [ ] **Battle playback UI** — replay recorded timeline, skippable.
  - *Depends on:* Battle sim
  - *Acceptance:* Playback matches the simulated result; result screen shown.

### Phase 4 — Arena, Progression & Online (M4 / MVP)

- [ ] **EOS cloud save** — sync `FPlayerSave` with conflict resolution.
  - *Depends on:* Local Save, EOS shell
  - *Acceptance:* Save round-trips to cloud; newest-valid-wins; backup kept.
- [x] **Ranked arena (snapshots)** — opponent selection by rating, submit result.
  - *Depends on:* Battle sim, EOS
  - *Acceptance:* Player runs a ranked match vs a snapshot; result recorded.
  - *Done 2026-06-14:* `URankedArenaLibrary::RunRankedMatch` (`Online/`) ties opponent
    selection → `SimulateBattle` → rating update → submit, all via the abstract
    `ULeaderboardService` (local impl now, EOS later). 3 tests pass. (See TechSpec §7.)
- [~] **EOS leaderboards + rating** — rating stat, ladder query.
  - *Acceptance:* Rating updates after matches; leaderboard reflects standings.
  - *Done 2026-06-14:* rating math (`URankingLibrary`, integer Elo-like) + service
    interface (`SelectOpponent`/`SubmitResult`/`GetTopEntries`) + `ULocalLeaderboardService`.
    **Pending:** the EOS-backed `ULeaderboardService` implementation (blocked on creds).
- [x] **Progression system** — player level, evolution unlocks, currency sinks.
  - *Acceptance:* Wins/breeding advance progression; unlocks gate content.
  - *Done 2026-06-14:* `UProgressionLibrary` (`Progression/`) — XP→level curve,
    ranked XP/coin rewards, data-driven level-gated unlocks (`FProgressionUnlockDef`
    + `Data/DT_Unlocks.json`). Currency sink is the breeding cost above. 2 tests pass.
    (UI to surface progression is editor-bound, pending.)
- [ ] **Onboarding + tutorial battle** — starter creature, scripted first win.
  - *Depends on:* Creation, Battle
  - *Acceptance:* New player reaches hub owning one creature, having won once.
- [ ] **Analytics events** — emit PRD success-metric events.
  - *Acceptance:* Session/battle/breeding/ranked events fire and are recorded.

### Phase 5 — Alpha hardening

- [ ] **Balance pass** — tune DataTables against playtests.
- [ ] **Content depth** — expand parts/genes/mutations toward "thousands of combos."
- [ ] **Performance** — hit 60 FPS on mid-range Android; mesh/material budgets.
- [ ] **Polish** — reveal/battle juice, empty/error states, accessibility checks.

## 4. Definition of done

> The bar a task must clear before it counts as done. Applies to every task.

- [ ] Meets its acceptance criteria
- [ ] Follows `rules.md`
- [ ] Tests written/updated and passing (esp. determinism for sim/genetics)
- [ ] Relevant docs updated (`schema.md`, `appflow.md`, `TechSpec.md`)
- [ ] `tracker.md` updated

## 5. Sequencing notes & prerequisites

- **EOS product setup** (Epic Dev Portal product, client credentials) must exist
  before Phase 0's EOS task — keep credentials out of the repo.
- **Test Android device(s)** representative of "mid-range" are needed by M0 to
  validate the *perf* target early, not at the end. Functional/UX validation runs on
  Windows in the meantime, so the missing device no longer blocks progress.
- **Determinism is a standing constraint:** no un-seeded RNG, no frame-time or
  float-order dependence in sim/genetics, and results must match **across x86-64
  (Windows) and arm64 (Android)** — keep the sim integer/fixed-point where practical
  and avoid platform-variant FP. Add the cross-platform repro test in M0 and keep it
  green.
- **Open PRD questions** (mutation count, part count, cooldown, rarity formula,
  max level, client vs server battles) gate finalizing M2–M4 balance; make
  documented default decisions to keep moving, revisit before Alpha.
- **Risk:** art/content throughput (solo dev) is the likeliest schedule risk —
  mitigated by procedural assembly and mesh reuse; track in `tracker.md`.
