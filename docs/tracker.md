# Tracker

> **Purpose:** The living status of the project — what's done, in progress,
> blocked, and next. This is the first file to read each session and the file
> to update after every task. Keep it current; treat it as the project's
> short-term memory.
> **Read it** at the start of every session.
> **Update it** after every task.

- **Last updated:** 2026-06-13
- **Current focus:** M0 setup — skeleton builds; next is the data layer (Phase 1) and the unblocked Phase 0 tasks (Windows build, CI/determinism harness). **Android + Windows are now co-primary V1 targets.**
- **Related:** `implementationplan.md` (the roadmap this draws from)

---

## Status legend

`todo` · `in-progress` · `blocked` · `review` · `done`

---

## Now (current sprint / focus)

| Task | Status | Notes |
|---|---|---|
| M0 — CI / determinism harness | done (Win64) | `Scripts/RunTests.ps1` (build+suite+exit code; verified 23/23→0) + `.github/workflows/ci.yml`; Android parity leg pending device |
| Phase 4 progression + Phase 2 breeding transaction | done | `UProgressionLibrary` (XP/level/unlocks) + `CanBreed`/`CommitBreed` (cost+cooldown); 4 tests **pass** |
| Phase 1 — DataTables for parts & genes | partial | Row structs + seed JSON + `ValidateGenome` done; **pending** `.uasset` import in-editor + startup validation |
| Phase 1 — Creature Assembly + Lab UI | todo | Editor-bound (UMG widgets) — needs an in-editor session; not headless-buildable |
| Resolve PRD open questions into default decisions | mostly | Resolved: rarity, mutation, battle model, rating, **breeding cooldown (4h)**. Open: max level/prestige, client-vs-server battles |

## Next up

> The headless C++ MVP logic is essentially complete (23/23 tests). What's left needs
> the editor or external unblocks:

- [ ] **Editor session** — Lab/breeding/arena/leaderboard **UI** (UMG), import seed JSON
  → `DT_*` `.uasset`s + wire startup validation, onboarding/tutorial, a startup map.
  C++ scaffolding (widget base classes, GameMode/HUD) can be written headless to support this.
- [ ] M0 — Windows packaging → runs to the hub (needs the startup map above)
- [ ] EOS implementation of `ULeaderboardService` + auth/cloud-save — **blocked on credentials**
- [ ] M0 — Android packaging + on-device perf + cross-arch determinism leg — **blocked on a device**
- [ ] Analytics provider + events (PRD success metrics)
- [ ] Balance/tuning pass on the MVP-default constants (rewards, cost, cooldown, rarity, rating)

> Note: full deterministic gameplay + economy loop is built & tested headless —
> genetics → breeding (cost/cooldown) → mutation → rarity → battle → ranked/rating →
> XP/level/unlocks → save (23/23 tests, incl. the battle determinism gate + a CI harness).
> The **online boundary is set** (`ULeaderboardService`), so EOS drops in behind it.
> From here, meaningful progress is **editor-bound** or **blocked** (device, EOS creds).

## Done

> Append completed tasks here (most recent first) so progress is visible.

- [x] [2026-06-14] M0 — CI / determinism harness: `Scripts/RunTests.ps1` (build + headless automation suite + CI pass/fail exit code; verified locally 23/23 → exit 0) and `.github/workflows/ci.yml` (self-hosted UE5 Windows runner). Android cross-arch parity leg pending a device. Also added the harness command to CLAUDE.md §5.
- [x] [2026-06-14] Phase 4 progression + Phase 2 breeding transaction: `UProgressionLibrary` (`Progression/`) — XP→level curve, ranked XP/coin rewards, data-driven level-gated unlocks (`FProgressionUnlockDef` + `Data/DT_Unlocks.json`); added `FPlayerSave.Xp`. `UBreedingLibrary::CanBreed`/`CommitBreed` — distinct/off-cooldown/affordable gate, deduct `BreedCost=50`, stamp `BreedCooldown=4h`, add offspring. Resolves the PRD breeding-cooldown question. 4 tests pass (Progression.XpAndLevel/Unlocks, Breeding.Transaction.CanBreed/Commit). Full suite 23/23.
- [x] [2026-06-14] Phase 4 (core) — Ranked arena: `URankedArenaLibrary::RunRankedMatch` + `URankingLibrary` (integer Elo-like) + abstract `ULeaderboardService` (`SubmitResult`/`SelectOpponent`/`GetTopEntries`) + `ULocalLeaderboardService` (`Online/`). Added ranked fields to `FPlayerSave`. Online-behind-interface boundary established (EOS impl deferred). 3 tests pass: RatingDelta, OpponentSelection, RankedMatch. Full suite 19/19. Also: set `bUseUnity=false` (test helpers with shared names collided under unity).
- [x] [2026-06-14] Phase 3 — Battle sim + Creature AI: `UBattleSimulator::SimulateBattle` + `UBattleAI::ChooseAction` + battle types (`Battle/`). Turn-based by Speed, integer damage, seeded `FRandomStream` Heavy-miss roll, `MaxRounds` cap, `FBattleResult` timeline (no render dep). Deterministic — **release gate test passes**. 4 tests pass: Determinism, StatsDecide, Termination, AIGoesForKill. Full suite 16/16.
- [x] [2026-06-14] Phase 2 — Breeding + Mutation + Rarity: `UBreedingLibrary` (`BreedGenomes`/`BreedCreatures`/`CalculateRarity`) + `UMutationLibrary::RollMutations` + `FMutationDef` (`Breeding/`, `Genetics/GeneticsRows.h`) + seed JSON. Deterministic seeded `FRandomStream`, stable draw order, integer per-mille. 4 tests pass: Inheritance, Determinism, MutationRate, Rarity. Full suite 12/12.
- [x] [2026-06-14] Phase 1 — Local Save subsystem: `USaveSubsystem` (GameInstance subsystem) + `UEvoSaveGame`/`FPlayerSave` (`Save/`). Atomic temp→rename writes, `MigrateIfNeeded` version hook, tagged SaveGame archive. 3 tests pass: RoundTrip, AtomicWrite, Migration. (Done before Lab UI — UMG is editor-bound; Save only needs the data structs.)
- [x] [2026-06-13] Phase 1 — Genetics System: `UGeneticsLibrary` (`ComputeDerivedStats`/`RecomputeDerivedStats`/`ResolveExpressedAllele`/`ValidateGenome`) + DataTable row types `FBodyPartDef`/`FGeneDef` (`EGeneDominance`) + seed JSON in `Data/`. Deterministic integer-only stat math. 4 automation tests pass: StatComputation, DominanceResolution, Determinism, ContentValidation.
- [x] [2026-06-13] Phase 1 — Core data structs: `FStatBlock`, `FGene`, `FPartAssignment`, `FGenome`, `FCreature` + enums (`ERarity`, `EBodyPartSlot`, `EGeneType`, `ECreatureStat`) as reflected `USTRUCT`s. Editor target compiles; the `EvolutionArena.Genetics.GenomeSerializationRoundTrip` automation test **passes** (serialize→deserialize preserves all fields).
- [x] [2026-06-13] Scaffolded the UE5 C++ project skeleton (`.uproject`, `Source/` primary game module + Build/Target rules, `Config/` inis, `.gitignore`); editor target **compiles clean** against UE 5.6 and the VS solution generates.
- [x] [2026-06-13] Relocated the living docs into `docs/` (CLAUDE.md kept at root); updated all CLAUDE.md paths and the `@docs/rules.md` import.
- [x] [2026-06-13] Drafted full doc set (TechSpec, appflow, design, schema, implementationplan, rules, tracker) from `PRD.md`.

---

## Blockers

| Blocker | Blocking | Owner | Since |
|---|---|---|---|
| EOS product/credentials not yet set up | M0 EOS integration | Owner | 2026-06-13 |
| No representative mid-range Android test device confirmed | M0 Android *perf* validation only (Windows now covers functional/UX testing) | Owner | 2026-06-13 |
| Launcher registry maps engine `5.6` → `C:\UE_5.6` but it's installed at `D:\UE_5.6` | Double-clicking the `.uproject` only — **workaround confirmed 2026-06-14:** launch `D:\UE_5.6\Engine\Binaries\Win64\UnrealEditor.exe <uproject>` (GUI editor opens clean, ~25s to editor; CLI builds also unaffected) | Owner | 2026-06-13 |

---

## Decisions log

> Append-only. Record decisions made during the build, with enough context that
> they don't get re-debated. Bigger architectural decisions also go in
> `TechSpec.md`.

- **2026-06-14** Progression/economy: **Level derived from `Xp`** (`1 + Xp/100`),
  ranked rewards (win > loss), **data-driven level-gated unlocks** (`FProgressionUnlockDef`).
  Breeding sink: `BreedCost=50` + `BreedCooldown=4h` (resolves PRD cooldown question).
  All integer; numbers are untuned MVP placeholders. (See TechSpec §7.)
- **2026-06-14** Online behind an abstract **`ULeaderboardService`** (local impl now,
  EOS subclass later — the only thing that changes to go live); ranked is async vs
  stored `FOpponentSnapshot` ghosts; rating is **integer Elo-like** (`URankingLibrary`,
  no floats). Realizes the rules.md "online behind interfaces" rule. (See TechSpec §7.)
- **2026-06-14** **`bUseUnity = false`** for the module: automation-test files reuse
  short file-local helper names (`MakePartTable`, `MakeFighter`…) in anonymous
  namespaces, which unity builds merge → collisions. Module is small, so per-file TUs
  are fine. (Alternative if unity is wanted later: uniquely-named namespaces / a shared
  test-helper header.)

- **2026-06-13** Living docs live in **`docs/`**; `CLAUDE.md` stays at the repo
  root (where Claude Code auto-loads it). All CLAUDE.md paths and the
  `@docs/rules.md` import point into `docs/`.
- **2026-06-13** Battles are **deterministic, seed-based client-side simulations**
  for MVP; server re-validation deferred to post-MVP. (See TechSpec §7.)
- **2026-06-13** All online features via **EOS** (auth, cloud save, leaderboards);
  no custom game server in MVP. (See TechSpec §7.)
- **2026-06-13** Ranked is **asynchronous vs opponent snapshots ("ghosts")** for
  MVP, not live head-to-head.
- **2026-06-13** Content is **data-driven** (DataTables/Data Assets); balance values
  never hardcoded.
- **2026-06-13** **Local SaveGame is source of truth**, EOS cloud is backup/sync;
  conflict = newest-valid-wins with backup kept.
- **2026-06-13** **Single primary game module** (`EvolutionArena`) with per-system
  subfolders (Genetics/Breeding/Battle/Save/Online), not separate UBT modules —
  simpler linking for a solo dev. Split out only if build times demand it.
- **2026-06-13** **Engine: UE 5.6**; `EngineAssociation="5.6"`. Mobile hardware
  targeting (Scalable), **arm64-only** Android, `r.MobileHDR=False` for the 60 FPS
  mid-range goal. (Aligns with TechSpec §8.)
- **2026-06-13** Gene expression model: `EGeneDominance` (Dominant=`Max`,
  Recessive=`Min`, Codominant=`(A+B)/2`); derived stats = `Σ parts + Σ (Stat-gene
  StatMapping × expressed allele)`, base zero, **integer-only** (cross-arch
  determinism). Genetics is a **pure library** now; a caching subsystem can layer on
  later. (See TechSpec §7.)
- **2026-06-14** Battle model (MVP): 1v1 turn-based, act in Speed order (ties→index),
  two actions (`Attack`, `Heavy` with seeded per-mille miss), integer damage
  (`Attack − Defense/2`), `MaxRounds` cap → decide by health. AI = go-for-the-kill
  utility (Heavy when lethal, no RNG in the choice). Sim has **no render dependency**;
  emits an `FBattleEvent` timeline for the playback UI. Single seeded stream →
  identical result cross-platform = the **determinism release gate**. (See TechSpec §7.)
- **2026-06-14** Breeding/mutation determinism: single seeded `FRandomStream`, RNG
  drawn over **ordered arrays** + mutation rows sorted by `FName::LexicalLess` (not
  `FastLess`), **integer per-mille** chance (`FMutationDef.ChancePermille`, refines
  schema's float `BaseChance`). Mendelian per-locus inheritance. Genome is
  deterministic for a seed; creature GUID/timestamp are not. Rarity = `Σ part
  rarities + 2×mutations` → thresholds (MVP default for the PRD rarity question).
  Mutation stat-effects deferred (genetics doesn't read mutations yet). (See TechSpec §7.)
- **2026-06-14** Save approach: data persists via a `USaveGame` (`UEvoSaveGame`)
  through UE's **tagged SaveGame archive** (resilient to field changes), written
  **atomically** (temp file → rename). `FPlayerSave.SaveVersion` + `MigrateIfNeeded`
  give the migration path. Build order: **Save before Lab UI** — UMG is editor-bound
  and not headless-verifiable, Save is pure C++ and depends only on the data structs.
  (See TechSpec §7.)
- **2026-06-13** Game C++ types must avoid engine-reserved names: renamed
  `EStatType` → **`ECreatureStat`** (engine defines a global `EStatType` in Core).
- **2026-06-13** Module organized by **system subfolders** (`Genetics/`, `Creature/`,
  `Tests/`); `EvolutionArena.Build.cs` adds `ModuleDirectory` to include paths so
  includes are module-root-relative (`#include "Genetics/Genome.h"`).
- **2026-06-13** Determinism/serialization is **proven by automation tests run
  headless** (`UnrealEditor-Cmd … Automation RunTests …`), not just by compiling.
  This is the pattern the CI determinism gate will use.
- **2026-06-13** **Android + Windows are co-primary V1 targets** (was Android-only).
  Single codebase; Enhanced Input for touch + mouse/keyboard/gamepad; responsive UI
  (portrait phone → landscape desktop). Battle sim must be deterministic **across
  x86-64 and arm64** — cross-platform parity added to the determinism gate. PRD,
  TechSpec, design, appflow, and implementation plan updated. (See TechSpec §7.)

---

## Changelog

> Brief, dated summary of notable changes shipped.

- **2026-06-14** First clean editor GUI open — launched via the explicit `D:\UE_5.6` path (registry workaround); reaches editor in ~25s, no errors, C++ classes + EOS SDK load. Empty default level; no project `Content/` yet.
- **2026-06-14** Added the CI / determinism harness (`Scripts/RunTests.ps1` + GitHub Actions workflow); verified 23/23 → exit 0. Headless C++ MVP logic now complete.
- **2026-06-14** Added progression (XP/level/unlocks) + breeding transaction (cost/cooldown) — the economy loop. Full suite: 23/23 green.
- **2026-06-14** Added Phase 4 ranked arena core (rating + opponent selection + match orchestration) behind the `ULeaderboardService` interface. Full suite: 19/19 green.
- **2026-06-14** Added Phase 3 deterministic battle sim + creature AI; determinism release gate passes. Full suite: 16/16 green.
- **2026-06-14** Added Phase 2 Breeding + Mutation + Rarity (deterministic, seeded); 4 tests pass. Full suite: 12/12 green.
- **2026-06-14** Added the Local Save subsystem (atomic write, versioned, migration hook); 3 save tests pass. Full suite: 8/8 green.
- **2026-06-13** Scope change: **Windows added as a co-primary V1 platform** alongside Android; propagated through PRD, TechSpec, design, appflow, and the implementation plan.
- **2026-06-13** Added the Genetics System (genome → deterministic derived stats) + DataTable row types + seed content; 4 genetics tests pass.
- **2026-06-13** Added the core genome data model (`FGenome`/`FCreature` + enums); serialize round-trip test passes.
- **2026-06-13** Scaffolded the UE5 C++ project; editor target builds against UE 5.6.
- **2026-06-13** Moved the living docs into `docs/`; repointed CLAUDE.md.
- **2026-06-13** Initialized the project document system from the PRD.
